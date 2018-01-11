char text_frag[] = "$OSG_GLSL_VERSION\n"
                   "\n"
                   "#pragma import_defines( BACKDROP_COLOR, SHADOW, OUTLINE, SIGNED_DISTANCE_FIELD, TEXTURE_DIMENSION, GLYPH_DIMENSION)\n"
                   "\n"
                   "#ifdef GL_ES\n"
                   "    #extension GL_OES_standard_derivatives : enable\n"
                   "    #ifndef GL_OES_standard_derivatives\n"
                   "        #undef SIGNED_DISTANCE_FIELD\n"
                   "    #endif\n"
                   "#endif\n"
                   "\n"
                   "#if !defined(GL_ES)\n"
                   "    #if __VERSION__>=400\n"
                   "        #define osg_TextureQueryLOD textureQueryLod\n"
                   "    #else\n"
                   "        #extension GL_ARB_texture_query_lod : enable\n"
                   "        #ifdef GL_ARB_texture_query_lod\n"
                   "            #define osg_TextureQueryLOD textureQueryLOD\n"
                   "        #endif\n"
                   "    #endif\n"
                   "#endif\n"
                   "\n"
                   "$OSG_PRECISION_FLOAT\n"
                   "\n"
                   "#if __VERSION__>=130\n"
                   "    #define TEXTURE texture\n"
                   "    #define TEXTURELOD textureLod\n"
                   "    out vec4 osg_FragColor;\n"
                   "#else\n"
                   "    #define TEXTURE texture2D\n"
                   "    #define TEXTURELOD texture2DLod\n"
                   "    #define osg_FragColor gl_FragColor\n"
                   "#endif\n"
                   "\n"
                   "\n"
                   "#if !defined(GL_ES) && __VERSION__>=130\n"
                   "    #define ALPHA r\n"
                   "    #define SDF g\n"
                   "#else\n"
                   "    #define ALPHA a\n"
                   "    #define SDF r\n"
                   "#endif\n"
                   "\n"
                   "\n"
                   "uniform sampler2D glyphTexture;\n"
                   "\n"
                   "$OSG_VARYING_IN vec2 texCoord;\n"
                   "$OSG_VARYING_IN vec4 vertexColor;\n"
                   "\n"
                   "#ifndef TEXTURE_DIMENSION\n"
                   "const float TEXTURE_DIMENSION = 1024.0;\n"
                   "#endif\n"
                   "\n"
                   "#ifndef GLYPH_DIMENSION\n"
                   "const float GLYPH_DIMENSION = 32.0;\n"
                   "#endif\n"
                   "\n"
                   "#ifdef SIGNED_DISTANCE_FIELD\n"
                   "\n"
                   "float distanceFromEdge(vec2 tc)\n"
                   "{\n"
                   "    float center_alpha = TEXTURELOD(glyphTexture, tc, 0.0).SDF;\n"
                   "    if (center_alpha==0.0) return -1.0;\n"
                   "\n"
                   "    //float distance_scale = (1.0/4.0)*1.41;\n"
                   "    float distance_scale = (1.0/6.0)*1.41;\n"
                   "    //float distance_scale = (1.0/8.0)*1.41;\n"
                   "\n"
                   "    return (center_alpha-0.5)*distance_scale;\n"
                   "}\n"
                   "\n"
                   "vec4 distanceFieldColorSample(float edge_distance, float blend_width, float  blend_half_width)\n"
                   "{\n"
                   "#ifdef OUTLINE\n"
                   "    float outline_width = OUTLINE*0.5;\n"
                   "    if (edge_distance>blend_half_width)\n"
                   "    {\n"
                   "        return vertexColor;\n"
                   "    }\n"
                   "    else if (edge_distance>-blend_half_width)\n"
                   "    {\n"
                   "        return mix(vertexColor, vec4(BACKDROP_COLOR.rgb, BACKDROP_COLOR.a*vertexColor.a), smoothstep(0.0, 1.0, (blend_half_width-edge_distance)/(blend_width)));\n"
                   "    }\n"
                   "    else if (edge_distance>(blend_half_width-outline_width))\n"
                   "    {\n"
                   "        return vec4(BACKDROP_COLOR.rgb, BACKDROP_COLOR.a*vertexColor.a);\n"
                   "    }\n"
                   "    else if (edge_distance>-(outline_width+blend_half_width))\n"
                   "    {\n"
                   "        return vec4(BACKDROP_COLOR.rgb, vertexColor.a * ((blend_half_width+outline_width+edge_distance)/blend_width));\n"
                   "    }\n"
                   "    else\n"
                   "    {\n"
                   "        return vec4(0.0, 0.0, 0.0, 0.0);\n"
                   "    }\n"
                   "#else\n"
                   "    if (edge_distance>blend_half_width)\n"
                   "    {\n"
                   "        return vertexColor;\n"
                   "    }\n"
                   "    else if (edge_distance>-blend_half_width)\n"
                   "    {\n"
                   "        return vec4(vertexColor.rgb, vertexColor.a * smoothstep(1.0, 0.0, (blend_half_width-edge_distance)/(blend_width)));\n"
                   "    }\n"
                   "    else\n"
                   "    {\n"
                   "        return vec4(0.0, 0.0, 0.0, 0.0);\n"
                   "    }\n"
                   "#endif\n"
                   "}\n"
                   "\n"
                   "vec4 textColor(vec2 src_texCoord)\n"
                   "{\n"
                   "    float sample_distance_scale = 0.75;\n"
                   "    vec2 dx = dFdx(src_texCoord)*sample_distance_scale;\n"
                   "    vec2 dy = dFdy(src_texCoord)*sample_distance_scale;\n"
                   "\n"
                   "\n"
                   "    float distance_across_pixel = length(dx+dy)*(TEXTURE_DIMENSION/GLYPH_DIMENSION);\n"
                   "\n"
                   "    // compute the appropriate number of samples required to avoid aliasing.\n"
                   "    int maxNumSamplesAcrossSide = 4;\n"
                   "\n"
                   "    int numSamplesX = int(TEXTURE_DIMENSION * length(dx));\n"
                   "    int numSamplesY = int(TEXTURE_DIMENSION * length(dy));\n"
                   "    if (numSamplesX<2) numSamplesX = 2;\n"
                   "    if (numSamplesY<2) numSamplesY = 2;\n"
                   "    if (numSamplesX>maxNumSamplesAcrossSide) numSamplesX = maxNumSamplesAcrossSide;\n"
                   "    if (numSamplesY>maxNumSamplesAcrossSide) numSamplesY = maxNumSamplesAcrossSide;\n"
                   "\n"
                   "\n"
                   "    vec2 delta_tx = dx/float(numSamplesX-1);\n"
                   "    vec2 delta_ty = dy/float(numSamplesY-1);\n"
                   "\n"
                   "    float numSamples = float(numSamplesX)*float(numSamplesY);\n"
                   "    float scale = 1.0/numSamples;\n"
                   "    vec4 total_color = vec4(0.0,0.0,0.0,0.0);\n"
                   "\n"
                   "    float blend_width = 1.5*distance_across_pixel/numSamples;\n"
                   "    float blend_half_width = blend_width*0.5;\n"
                   "\n"
                   "    // check whether fragment is wholly within or outwith glyph body+outline\n"
                   "    float cd = distanceFromEdge(src_texCoord); // central distance (distance from center to edge)\n"
                   "    if (cd-blend_half_width>distance_across_pixel) return vertexColor; // pixel fully within glyph body\n"
                   "\n"
                   "    #ifdef OUTLINE\n"
                   "    float outline_width = OUTLINE*0.5;\n"
                   "    if ((-cd-outline_width-blend_half_width)>distance_across_pixel) return vec4(0.0, 0.0, 0.0, 0.0); // pixel fully outside outline+glyph body\n"
                   "    #else\n"
                   "    if (-cd-blend_half_width>distance_across_pixel) return vec4(0.0, 0.0, 0.0, 0.0); // pixel fully outside glyph body\n"
                   "    #endif\n"
                   "\n"
                   "\n"
                   "    // use multi-sampling to provide high quality antialised fragments\n"
                   "    vec2 origin = src_texCoord - dx*0.5 - dy*0.5;\n"
                   "    for(;numSamplesY>0; --numSamplesY)\n"
                   "    {\n"
                   "        vec2 pos = origin;\n"
                   "        int numX = numSamplesX;\n"
                   "        for(;numX>0; --numX)\n"
                   "        {\n"
                   "            vec4 c = distanceFieldColorSample(distanceFromEdge(pos), blend_width, blend_half_width);\n"
                   "            total_color = total_color + c * c.a;\n"
                   "            pos += delta_tx;\n"
                   "        }\n"
                   "        origin += delta_ty;\n"
                   "    }\n"
                   "\n"
                   "    total_color.rgb /= total_color.a;\n"
                   "    total_color.a *= scale;\n"
                   "\n"
                   "    return total_color;\n"
                   "}\n"
                   "\n"
                   "#else\n"
                   "\n"
                   "vec4 textColor(vec2 src_texCoord)\n"
                   "{\n"
                   "\n"
                   "#ifdef OUTLINE\n"
                   "\n"
                   "    float alpha = TEXTURE(glyphTexture, src_texCoord).ALPHA;\n"
                   "    float delta_tc = 1.6*OUTLINE*GLYPH_DIMENSION/TEXTURE_DIMENSION;\n"
                   "\n"
                   "    float outline_alpha = alpha;\n"
                   "    vec2 origin = src_texCoord-vec2(delta_tc*0.5, delta_tc*0.5);\n"
                   "\n"
                   "    float numSamples = 3.0;\n"
                   "    delta_tc = delta_tc/(numSamples-1.0);\n"
                   "\n"
                   "    float background_alpha = 1.0;\n"
                   "\n"
                   "    for(float i=0.0; i<numSamples; ++i)\n"
                   "    {\n"
                   "        for(float j=0.0; j<numSamples; ++j)\n"
                   "        {\n"
                   "            float local_alpha = TEXTURE(glyphTexture, origin + vec2(i*delta_tc, j*delta_tc)).ALPHA;\n"
                   "            outline_alpha = max(outline_alpha, local_alpha);\n"
                   "            background_alpha = background_alpha * (1.0-local_alpha);\n"
                   "        }\n"
                   "    }\n"
                   "\n"
                   "    #ifdef osg_TextureQueryLOD\n"
                   "        float mipmapLevel = osg_TextureQueryLOD(glyphTexture, src_texCoord).x;\n"
                   "        if (mipmapLevel<1.0)\n"
                   "        {\n"
                   "            outline_alpha = mix(1.0-background_alpha, outline_alpha, mipmapLevel/1.0);\n"
                   "        }\n"
                   "    #endif\n"
                   "\n"
                   "    if (outline_alpha<alpha) outline_alpha = alpha;\n"
                   "    if (outline_alpha>1.0) outline_alpha = 1.0;\n"
                   "\n"
                   "    if (outline_alpha==0.0) return vec4(0.0, 0.0, 0.0, 0.0); // outside glyph and outline\n"
                   "\n"
                   "    vec4 color = mix(BACKDROP_COLOR, vertexColor, smoothstep(0.0, 1.0, alpha));\n"
                   "    color.a = vertexColor.a * smoothstep(0.0, 1.0, outline_alpha);\n"
                   "\n"
                   "    return color;\n"
                   "\n"
                   "#else\n"
                   "\n"
                   "    float alpha = TEXTURE(glyphTexture, src_texCoord).ALPHA;\n"
                   "    if (alpha==0.0) vec4(0.0, 0.0, 0.0, 0.0);\n"
                   "    return vec4(vertexColor.rgb, vertexColor.a * alpha);\n"
                   "\n"
                   "#endif\n"
                   "}\n"
                   "\n"
                   "#endif\n"
                   "\n"
                   "\n"
                   "void main(void)\n"
                   "{\n"
                   "    if (texCoord.x<0.0 && texCoord.y<0.0)\n"
                   "    {\n"
                   "        osg_FragColor = vertexColor;\n"
                   "        return;\n"
                   "    }\n"
                   "\n"
                   "#ifdef SHADOW\n"
                   "    float scale = -1.0*GLYPH_DIMENSION/TEXTURE_DIMENSION;\n"
                   "    vec2 delta_tc = SHADOW*scale;\n"
                   "    vec4 shadow_color = textColor(texCoord+delta_tc);\n"
                   "    shadow_color.rgb = BACKDROP_COLOR.rgb;\n"
                   "\n"
                   "    vec4 glyph_color = textColor(texCoord);\n"
                   "    vec4 color = mix(shadow_color, glyph_color, glyph_color.a);\n"
                   "#else\n"
                   "    vec4 color = textColor(texCoord);\n"
                   "#endif\n"
                   "\n"
                   "    if (color.a==0.0) discard;\n"
                   "\n"
                   "    osg_FragColor = color;\n"
                   "}\n"
                   "\n";
