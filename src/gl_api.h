#pragma once
#ifndef APIENTRY
#define APIENTRY
#endif
#ifndef GL_VERSION_1_0
#define GL_VERSION_1_0 1
typedef void GLvoid;
typedef unsigned int GLenum;
#include "khrplatform.h"
typedef khronos_float_t GLfloat;
typedef int GLint;
typedef int GLsizei;
typedef unsigned int GLbitfield;
typedef double GLdouble;
typedef unsigned int GLuint;
typedef unsigned char GLboolean;
typedef khronos_uint8_t GLubyte;
#define GL_DEPTH_BUFFER_BIT               0x00000100
#define GL_STENCIL_BUFFER_BIT             0x00000400
#define GL_COLOR_BUFFER_BIT               0x00004000
#define GL_FALSE                          0
#define GL_TRUE                           1
#define GL_POINTS                         0x0000
#define GL_LINES                          0x0001
#define GL_LINE_LOOP                      0x0002
#define GL_LINE_STRIP                     0x0003
#define GL_TRIANGLES                      0x0004
#define GL_TRIANGLE_STRIP                 0x0005
#define GL_TRIANGLE_FAN                   0x0006
#define GL_QUADS                          0x0007
#define GL_NEVER                          0x0200
#define GL_LESS                           0x0201
#define GL_EQUAL                          0x0202
#define GL_LEQUAL                         0x0203
#define GL_GREATER                        0x0204
#define GL_NOTEQUAL                       0x0205
#define GL_GEQUAL                         0x0206
#define GL_ALWAYS                         0x0207
#define GL_ZERO                           0
#define GL_ONE                            1
#define GL_SRC_COLOR                      0x0300
#define GL_ONE_MINUS_SRC_COLOR            0x0301
#define GL_SRC_ALPHA                      0x0302
#define GL_ONE_MINUS_SRC_ALPHA            0x0303
#define GL_DST_ALPHA                      0x0304
#define GL_ONE_MINUS_DST_ALPHA            0x0305
#define GL_DST_COLOR                      0x0306
#define GL_ONE_MINUS_DST_COLOR            0x0307
#define GL_SRC_ALPHA_SATURATE             0x0308
#define GL_NONE                           0
#define GL_FRONT_LEFT                     0x0400
#define GL_FRONT_RIGHT                    0x0401
#define GL_BACK_LEFT                      0x0402
#define GL_BACK_RIGHT                     0x0403
#define GL_FRONT                          0x0404
#define GL_BACK                           0x0405
#define GL_LEFT                           0x0406
#define GL_RIGHT                          0x0407
#define GL_FRONT_AND_BACK                 0x0408
#define GL_NO_ERROR                       0
#define GL_INVALID_ENUM                   0x0500
#define GL_INVALID_VALUE                  0x0501
#define GL_INVALID_OPERATION              0x0502
#define GL_OUT_OF_MEMORY                  0x0505
#define GL_CW                             0x0900
#define GL_CCW                            0x0901
#define GL_POINT_SIZE                     0x0B11
#define GL_POINT_SIZE_RANGE               0x0B12
#define GL_POINT_SIZE_GRANULARITY         0x0B13
#define GL_LINE_SMOOTH                    0x0B20
#define GL_LINE_WIDTH                     0x0B21
#define GL_LINE_WIDTH_RANGE               0x0B22
#define GL_LINE_WIDTH_GRANULARITY         0x0B23
#define GL_POLYGON_MODE                   0x0B40
#define GL_POLYGON_SMOOTH                 0x0B41
#define GL_CULL_FACE                      0x0B44
#define GL_CULL_FACE_MODE                 0x0B45
#define GL_FRONT_FACE                     0x0B46
#define GL_DEPTH_RANGE                    0x0B70
#define GL_DEPTH_TEST                     0x0B71
#define GL_DEPTH_WRITEMASK                0x0B72
#define GL_DEPTH_CLEAR_VALUE              0x0B73
#define GL_DEPTH_FUNC                     0x0B74
#define GL_STENCIL_TEST                   0x0B90
#define GL_STENCIL_CLEAR_VALUE            0x0B91
#define GL_STENCIL_FUNC                   0x0B92
#define GL_STENCIL_VALUE_MASK             0x0B93
#define GL_STENCIL_FAIL                   0x0B94
#define GL_STENCIL_PASS_DEPTH_FAIL        0x0B95
#define GL_STENCIL_PASS_DEPTH_PASS        0x0B96
#define GL_STENCIL_REF                    0x0B97
#define GL_STENCIL_WRITEMASK              0x0B98
#define GL_VIEWPORT                       0x0BA2
#define GL_DITHER                         0x0BD0
#define GL_BLEND_DST                      0x0BE0
#define GL_BLEND_SRC                      0x0BE1
#define GL_BLEND                          0x0BE2
#define GL_LOGIC_OP_MODE                  0x0BF0
#define GL_DRAW_BUFFER                    0x0C01
#define GL_READ_BUFFER                    0x0C02
#define GL_SCISSOR_BOX                    0x0C10
#define GL_SCISSOR_TEST                   0x0C11
#define GL_COLOR_CLEAR_VALUE              0x0C22
#define GL_COLOR_WRITEMASK                0x0C23
#define GL_DOUBLEBUFFER                   0x0C32
#define GL_STEREO                         0x0C33
#define GL_LINE_SMOOTH_HINT               0x0C52
#define GL_POLYGON_SMOOTH_HINT            0x0C53
#define GL_UNPACK_SWAP_BYTES              0x0CF0
#define GL_UNPACK_LSB_FIRST               0x0CF1
#define GL_UNPACK_ROW_LENGTH              0x0CF2
#define GL_UNPACK_SKIP_ROWS               0x0CF3
#define GL_UNPACK_SKIP_PIXELS             0x0CF4
#define GL_UNPACK_ALIGNMENT               0x0CF5
#define GL_PACK_SWAP_BYTES                0x0D00
#define GL_PACK_LSB_FIRST                 0x0D01
#define GL_PACK_ROW_LENGTH                0x0D02
#define GL_PACK_SKIP_ROWS                 0x0D03
#define GL_PACK_SKIP_PIXELS               0x0D04
#define GL_PACK_ALIGNMENT                 0x0D05
#define GL_MAX_TEXTURE_SIZE               0x0D33
#define GL_MAX_VIEWPORT_DIMS              0x0D3A
#define GL_SUBPIXEL_BITS                  0x0D50
#define GL_TEXTURE_1D                     0x0DE0
#define GL_TEXTURE_2D                     0x0DE1
#define GL_TEXTURE_WIDTH                  0x1000
#define GL_TEXTURE_HEIGHT                 0x1001
#define GL_TEXTURE_BORDER_COLOR           0x1004
#define GL_DONT_CARE                      0x1100
#define GL_FASTEST                        0x1101
#define GL_NICEST                         0x1102
#define GL_BYTE                           0x1400
#define GL_UNSIGNED_BYTE                  0x1401
#define GL_SHORT                          0x1402
#define GL_UNSIGNED_SHORT                 0x1403
#define GL_INT                            0x1404
#define GL_UNSIGNED_INT                   0x1405
#define GL_FLOAT                          0x1406
#define GL_STACK_OVERFLOW                 0x0503
#define GL_STACK_UNDERFLOW                0x0504
#define GL_CLEAR                          0x1500
#define GL_AND                            0x1501
#define GL_AND_REVERSE                    0x1502
#define GL_COPY                           0x1503
#define GL_AND_INVERTED                   0x1504
#define GL_NOOP                           0x1505
#define GL_XOR                            0x1506
#define GL_OR                             0x1507
#define GL_NOR                            0x1508
#define GL_EQUIV                          0x1509
#define GL_INVERT                         0x150A
#define GL_OR_REVERSE                     0x150B
#define GL_COPY_INVERTED                  0x150C
#define GL_OR_INVERTED                    0x150D
#define GL_NAND                           0x150E
#define GL_SET                            0x150F
#define GL_TEXTURE                        0x1702
#define GL_COLOR                          0x1800
#define GL_DEPTH                          0x1801
#define GL_STENCIL                        0x1802
#define GL_STENCIL_INDEX                  0x1901
#define GL_DEPTH_COMPONENT                0x1902
#define GL_RED                            0x1903
#define GL_GREEN                          0x1904
#define GL_BLUE                           0x1905
#define GL_ALPHA                          0x1906
#define GL_RGB                            0x1907
#define GL_RGBA                           0x1908
#define GL_POINT                          0x1B00
#define GL_LINE                           0x1B01
#define GL_FILL                           0x1B02
#define GL_KEEP                           0x1E00
#define GL_REPLACE                        0x1E01
#define GL_INCR                           0x1E02
#define GL_DECR                           0x1E03
#define GL_VENDOR                         0x1F00
#define GL_RENDERER                       0x1F01
#define GL_VERSION                        0x1F02
#define GL_EXTENSIONS                     0x1F03
#define GL_NEAREST                        0x2600
#define GL_LINEAR                         0x2601
#define GL_NEAREST_MIPMAP_NEAREST         0x2700
#define GL_LINEAR_MIPMAP_NEAREST          0x2701
#define GL_NEAREST_MIPMAP_LINEAR          0x2702
#define GL_LINEAR_MIPMAP_LINEAR           0x2703
#define GL_TEXTURE_MAG_FILTER             0x2800
#define GL_TEXTURE_MIN_FILTER             0x2801
#define GL_TEXTURE_WRAP_S                 0x2802
#define GL_TEXTURE_WRAP_T                 0x2803
#define GL_REPEAT                         0x2901
#ifdef GL_GLEXT_PROTOTYPES
#endif
#endif /* GL_VERSION_1_0 */

#ifndef GL_VERSION_1_1
#define GL_VERSION_1_1 1
typedef khronos_float_t GLclampf;
typedef double GLclampd;
#define GL_COLOR_LOGIC_OP                 0x0BF2
#define GL_POLYGON_OFFSET_UNITS           0x2A00
#define GL_POLYGON_OFFSET_POINT           0x2A01
#define GL_POLYGON_OFFSET_LINE            0x2A02
#define GL_POLYGON_OFFSET_FILL            0x8037
#define GL_POLYGON_OFFSET_FACTOR          0x8038
#define GL_TEXTURE_BINDING_1D             0x8068
#define GL_TEXTURE_BINDING_2D             0x8069
#define GL_TEXTURE_INTERNAL_FORMAT        0x1003
#define GL_TEXTURE_RED_SIZE               0x805C
#define GL_TEXTURE_GREEN_SIZE             0x805D
#define GL_TEXTURE_BLUE_SIZE              0x805E
#define GL_TEXTURE_ALPHA_SIZE             0x805F
#define GL_DOUBLE                         0x140A
#define GL_PROXY_TEXTURE_1D               0x8063
#define GL_PROXY_TEXTURE_2D               0x8064
#define GL_R3_G3_B2                       0x2A10
#define GL_RGB4                           0x804F
#define GL_RGB5                           0x8050
#define GL_RGB8                           0x8051
#define GL_RGB10                          0x8052
#define GL_RGB12                          0x8053
#define GL_RGB16                          0x8054
#define GL_RGBA2                          0x8055
#define GL_RGBA4                          0x8056
#define GL_RGB5_A1                        0x8057
#define GL_RGBA8                          0x8058
#define GL_RGB10_A2                       0x8059
#define GL_RGBA12                         0x805A
#define GL_RGBA16                         0x805B
#define GL_VERTEX_ARRAY                   0x8074
#ifdef GL_GLEXT_PROTOTYPES
#endif
#endif /* GL_VERSION_1_1 */

#ifndef GL_VERSION_1_2
#define GL_VERSION_1_2 1
#define GL_UNSIGNED_BYTE_3_3_2            0x8032
#define GL_UNSIGNED_SHORT_4_4_4_4         0x8033
#define GL_UNSIGNED_SHORT_5_5_5_1         0x8034
#define GL_UNSIGNED_INT_8_8_8_8           0x8035
#define GL_UNSIGNED_INT_10_10_10_2        0x8036
#define GL_TEXTURE_BINDING_3D             0x806A
#define GL_PACK_SKIP_IMAGES               0x806B
#define GL_PACK_IMAGE_HEIGHT              0x806C
#define GL_UNPACK_SKIP_IMAGES             0x806D
#define GL_UNPACK_IMAGE_HEIGHT            0x806E
#define GL_TEXTURE_3D                     0x806F
#define GL_PROXY_TEXTURE_3D               0x8070
#define GL_TEXTURE_DEPTH                  0x8071
#define GL_TEXTURE_WRAP_R                 0x8072
#define GL_MAX_3D_TEXTURE_SIZE            0x8073
#define GL_UNSIGNED_BYTE_2_3_3_REV        0x8362
#define GL_UNSIGNED_SHORT_5_6_5           0x8363
#define GL_UNSIGNED_SHORT_5_6_5_REV       0x8364
#define GL_UNSIGNED_SHORT_4_4_4_4_REV     0x8365
#define GL_UNSIGNED_SHORT_1_5_5_5_REV     0x8366
#define GL_UNSIGNED_INT_8_8_8_8_REV       0x8367
#define GL_UNSIGNED_INT_2_10_10_10_REV    0x8368
#define GL_BGR                            0x80E0
#define GL_BGRA                           0x80E1
#define GL_MAX_ELEMENTS_VERTICES          0x80E8
#define GL_MAX_ELEMENTS_INDICES           0x80E9
#define GL_CLAMP_TO_EDGE                  0x812F
#define GL_TEXTURE_MIN_LOD                0x813A
#define GL_TEXTURE_MAX_LOD                0x813B
#define GL_TEXTURE_BASE_LEVEL             0x813C
#define GL_TEXTURE_MAX_LEVEL              0x813D
#define GL_SMOOTH_POINT_SIZE_RANGE        0x0B12
#define GL_SMOOTH_POINT_SIZE_GRANULARITY  0x0B13
#define GL_SMOOTH_LINE_WIDTH_RANGE        0x0B22
#define GL_SMOOTH_LINE_WIDTH_GRANULARITY  0x0B23
#define GL_ALIASED_LINE_WIDTH_RANGE       0x846E
#ifdef GL_GLEXT_PROTOTYPES
#endif
#endif /* GL_VERSION_1_2 */

#ifndef GL_VERSION_1_3
#define GL_VERSION_1_3 1
#define GL_TEXTURE0                       0x84C0
#define GL_TEXTURE1                       0x84C1
#define GL_TEXTURE2                       0x84C2
#define GL_TEXTURE3                       0x84C3
#define GL_TEXTURE4                       0x84C4
#define GL_TEXTURE5                       0x84C5
#define GL_TEXTURE6                       0x84C6
#define GL_TEXTURE7                       0x84C7
#define GL_TEXTURE8                       0x84C8
#define GL_TEXTURE9                       0x84C9
#define GL_TEXTURE10                      0x84CA
#define GL_TEXTURE11                      0x84CB
#define GL_TEXTURE12                      0x84CC
#define GL_TEXTURE13                      0x84CD
#define GL_TEXTURE14                      0x84CE
#define GL_TEXTURE15                      0x84CF
#define GL_TEXTURE16                      0x84D0
#define GL_TEXTURE17                      0x84D1
#define GL_TEXTURE18                      0x84D2
#define GL_TEXTURE19                      0x84D3
#define GL_TEXTURE20                      0x84D4
#define GL_TEXTURE21                      0x84D5
#define GL_TEXTURE22                      0x84D6
#define GL_TEXTURE23                      0x84D7
#define GL_TEXTURE24                      0x84D8
#define GL_TEXTURE25                      0x84D9
#define GL_TEXTURE26                      0x84DA
#define GL_TEXTURE27                      0x84DB
#define GL_TEXTURE28                      0x84DC
#define GL_TEXTURE29                      0x84DD
#define GL_TEXTURE30                      0x84DE
#define GL_TEXTURE31                      0x84DF
#define GL_ACTIVE_TEXTURE                 0x84E0
#define GL_MULTISAMPLE                    0x809D
#define GL_SAMPLE_ALPHA_TO_COVERAGE       0x809E
#define GL_SAMPLE_ALPHA_TO_ONE            0x809F
#define GL_SAMPLE_COVERAGE                0x80A0
#define GL_SAMPLE_BUFFERS                 0x80A8
#define GL_SAMPLES                        0x80A9
#define GL_SAMPLE_COVERAGE_VALUE          0x80AA
#define GL_SAMPLE_COVERAGE_INVERT         0x80AB
#define GL_TEXTURE_CUBE_MAP               0x8513
#define GL_TEXTURE_BINDING_CUBE_MAP       0x8514
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X    0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X    0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y    0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y    0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z    0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z    0x851A
#define GL_PROXY_TEXTURE_CUBE_MAP         0x851B
#define GL_MAX_CUBE_MAP_TEXTURE_SIZE      0x851C
#define GL_COMPRESSED_RGB                 0x84ED
#define GL_COMPRESSED_RGBA                0x84EE
#define GL_TEXTURE_COMPRESSION_HINT       0x84EF
#define GL_TEXTURE_COMPRESSED_IMAGE_SIZE  0x86A0
#define GL_TEXTURE_COMPRESSED             0x86A1
#define GL_NUM_COMPRESSED_TEXTURE_FORMATS 0x86A2
#define GL_COMPRESSED_TEXTURE_FORMATS     0x86A3
#define GL_CLAMP_TO_BORDER                0x812D
#ifdef GL_GLEXT_PROTOTYPES
#endif
#endif /* GL_VERSION_1_3 */

#ifndef GL_VERSION_1_4
#define GL_VERSION_1_4 1
#define GL_BLEND_DST_RGB                  0x80C8
#define GL_BLEND_SRC_RGB                  0x80C9
#define GL_BLEND_DST_ALPHA                0x80CA
#define GL_BLEND_SRC_ALPHA                0x80CB
#define GL_POINT_FADE_THRESHOLD_SIZE      0x8128
#define GL_DEPTH_COMPONENT16              0x81A5
#define GL_DEPTH_COMPONENT24              0x81A6
#define GL_DEPTH_COMPONENT32              0x81A7
#define GL_MIRRORED_REPEAT                0x8370
#define GL_MAX_TEXTURE_LOD_BIAS           0x84FD
#define GL_TEXTURE_LOD_BIAS               0x8501
#define GL_INCR_WRAP                      0x8507
#define GL_DECR_WRAP                      0x8508
#define GL_TEXTURE_DEPTH_SIZE             0x884A
#define GL_TEXTURE_COMPARE_MODE           0x884C
#define GL_TEXTURE_COMPARE_FUNC           0x884D
#define GL_BLEND_COLOR                    0x8005
#define GL_BLEND_EQUATION                 0x8009
#define GL_CONSTANT_COLOR                 0x8001
#define GL_ONE_MINUS_CONSTANT_COLOR       0x8002
#define GL_CONSTANT_ALPHA                 0x8003
#define GL_ONE_MINUS_CONSTANT_ALPHA       0x8004
#define GL_FUNC_ADD                       0x8006
#define GL_FUNC_REVERSE_SUBTRACT          0x800B
#define GL_FUNC_SUBTRACT                  0x800A
#define GL_MIN                            0x8007
#define GL_MAX                            0x8008
#ifdef GL_GLEXT_PROTOTYPES
#endif
#endif /* GL_VERSION_1_4 */

#ifndef GL_VERSION_1_5
#define GL_VERSION_1_5 1
typedef khronos_ssize_t GLsizeiptr;
typedef khronos_intptr_t GLintptr;
#define GL_BUFFER_SIZE                    0x8764
#define GL_BUFFER_USAGE                   0x8765
#define GL_QUERY_COUNTER_BITS             0x8864
#define GL_CURRENT_QUERY                  0x8865
#define GL_QUERY_RESULT                   0x8866
#define GL_QUERY_RESULT_AVAILABLE         0x8867
#define GL_ARRAY_BUFFER                   0x8892
#define GL_ELEMENT_ARRAY_BUFFER           0x8893
#define GL_ARRAY_BUFFER_BINDING           0x8894
#define GL_ELEMENT_ARRAY_BUFFER_BINDING   0x8895
#define GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING 0x889F
#define GL_READ_ONLY                      0x88B8
#define GL_WRITE_ONLY                     0x88B9
#define GL_READ_WRITE                     0x88BA
#define GL_BUFFER_ACCESS                  0x88BB
#define GL_BUFFER_MAPPED                  0x88BC
#define GL_BUFFER_MAP_POINTER             0x88BD
#define GL_STREAM_DRAW                    0x88E0
#define GL_STREAM_READ                    0x88E1
#define GL_STREAM_COPY                    0x88E2
#define GL_STATIC_DRAW                    0x88E4
#define GL_STATIC_READ                    0x88E5
#define GL_STATIC_COPY                    0x88E6
#define GL_DYNAMIC_DRAW                   0x88E8
#define GL_DYNAMIC_READ                   0x88E9
#define GL_DYNAMIC_COPY                   0x88EA
#define GL_SAMPLES_PASSED                 0x8914
#define GL_SRC1_ALPHA                     0x8589
#ifdef GL_GLEXT_PROTOTYPES
#endif
#endif /* GL_VERSION_1_5 */

#ifndef GL_VERSION_2_0
#define GL_VERSION_2_0 1
typedef char GLchar;
typedef khronos_int16_t GLshort;
typedef khronos_int8_t GLbyte;
typedef khronos_uint16_t GLushort;
#define GL_BLEND_EQUATION_RGB             0x8009
#define GL_VERTEX_ATTRIB_ARRAY_ENABLED    0x8622
#define GL_VERTEX_ATTRIB_ARRAY_SIZE       0x8623
#define GL_VERTEX_ATTRIB_ARRAY_STRIDE     0x8624
#define GL_VERTEX_ATTRIB_ARRAY_TYPE       0x8625
#define GL_CURRENT_VERTEX_ATTRIB          0x8626
#define GL_VERTEX_PROGRAM_POINT_SIZE      0x8642
#define GL_VERTEX_ATTRIB_ARRAY_POINTER    0x8645
#define GL_STENCIL_BACK_FUNC              0x8800
#define GL_STENCIL_BACK_FAIL              0x8801
#define GL_STENCIL_BACK_PASS_DEPTH_FAIL   0x8802
#define GL_STENCIL_BACK_PASS_DEPTH_PASS   0x8803
#define GL_MAX_DRAW_BUFFERS               0x8824
#define GL_DRAW_BUFFER0                   0x8825
#define GL_DRAW_BUFFER1                   0x8826
#define GL_DRAW_BUFFER2                   0x8827
#define GL_DRAW_BUFFER3                   0x8828
#define GL_DRAW_BUFFER4                   0x8829
#define GL_DRAW_BUFFER5                   0x882A
#define GL_DRAW_BUFFER6                   0x882B
#define GL_DRAW_BUFFER7                   0x882C
#define GL_DRAW_BUFFER8                   0x882D
#define GL_DRAW_BUFFER9                   0x882E
#define GL_DRAW_BUFFER10                  0x882F
#define GL_DRAW_BUFFER11                  0x8830
#define GL_DRAW_BUFFER12                  0x8831
#define GL_DRAW_BUFFER13                  0x8832
#define GL_DRAW_BUFFER14                  0x8833
#define GL_DRAW_BUFFER15                  0x8834
#define GL_BLEND_EQUATION_ALPHA           0x883D
#define GL_MAX_VERTEX_ATTRIBS             0x8869
#define GL_VERTEX_ATTRIB_ARRAY_NORMALIZED 0x886A
#define GL_MAX_TEXTURE_IMAGE_UNITS        0x8872
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_VERTEX_SHADER                  0x8B31
#define GL_MAX_FRAGMENT_UNIFORM_COMPONENTS 0x8B49
#define GL_MAX_VERTEX_UNIFORM_COMPONENTS  0x8B4A
#define GL_MAX_VARYING_FLOATS             0x8B4B
#define GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS 0x8B4C
#define GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS 0x8B4D
#define GL_SHADER_TYPE                    0x8B4F
#define GL_FLOAT_VEC2                     0x8B50
#define GL_FLOAT_VEC3                     0x8B51
#define GL_FLOAT_VEC4                     0x8B52
#define GL_INT_VEC2                       0x8B53
#define GL_INT_VEC3                       0x8B54
#define GL_INT_VEC4                       0x8B55
#define GL_BOOL                           0x8B56
#define GL_BOOL_VEC2                      0x8B57
#define GL_BOOL_VEC3                      0x8B58
#define GL_BOOL_VEC4                      0x8B59
#define GL_FLOAT_MAT2                     0x8B5A
#define GL_FLOAT_MAT3                     0x8B5B
#define GL_FLOAT_MAT4                     0x8B5C
#define GL_SAMPLER_1D                     0x8B5D
#define GL_SAMPLER_2D                     0x8B5E
#define GL_SAMPLER_3D                     0x8B5F
#define GL_SAMPLER_CUBE                   0x8B60
#define GL_SAMPLER_1D_SHADOW              0x8B61
#define GL_SAMPLER_2D_SHADOW              0x8B62
#define GL_DELETE_STATUS                  0x8B80
#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82
#define GL_VALIDATE_STATUS                0x8B83
#define GL_INFO_LOG_LENGTH                0x8B84
#define GL_ATTACHED_SHADERS               0x8B85
#define GL_ACTIVE_UNIFORMS                0x8B86
#define GL_ACTIVE_UNIFORM_MAX_LENGTH      0x8B87
#define GL_SHADER_SOURCE_LENGTH           0x8B88
#define GL_ACTIVE_ATTRIBUTES              0x8B89
#define GL_ACTIVE_ATTRIBUTE_MAX_LENGTH    0x8B8A
#define GL_FRAGMENT_SHADER_DERIVATIVE_HINT 0x8B8B
#define GL_SHADING_LANGUAGE_VERSION       0x8B8C
#define GL_CURRENT_PROGRAM                0x8B8D
#define GL_POINT_SPRITE_COORD_ORIGIN      0x8CA0
#define GL_LOWER_LEFT                     0x8CA1
#define GL_UPPER_LEFT                     0x8CA2
#define GL_STENCIL_BACK_REF               0x8CA3
#define GL_STENCIL_BACK_VALUE_MASK        0x8CA4
#define GL_STENCIL_BACK_WRITEMASK         0x8CA5
#ifdef GL_GLEXT_PROTOTYPES
#endif
#endif /* GL_VERSION_2_0 */

#ifndef GL_VERSION_2_1
#define GL_VERSION_2_1 1
#define GL_PIXEL_PACK_BUFFER              0x88EB
#define GL_PIXEL_UNPACK_BUFFER            0x88EC
#define GL_PIXEL_PACK_BUFFER_BINDING      0x88ED
#define GL_PIXEL_UNPACK_BUFFER_BINDING    0x88EF
#define GL_FLOAT_MAT2x3                   0x8B65
#define GL_FLOAT_MAT2x4                   0x8B66
#define GL_FLOAT_MAT3x2                   0x8B67
#define GL_FLOAT_MAT3x4                   0x8B68
#define GL_FLOAT_MAT4x2                   0x8B69
#define GL_FLOAT_MAT4x3                   0x8B6A
#define GL_SRGB                           0x8C40
#define GL_SRGB8                          0x8C41
#define GL_SRGB_ALPHA                     0x8C42
#define GL_SRGB8_ALPHA8                   0x8C43
#define GL_COMPRESSED_SRGB                0x8C48
#define GL_COMPRESSED_SRGB_ALPHA          0x8C49
#ifdef GL_GLEXT_PROTOTYPES
#endif
#endif /* GL_VERSION_2_1 */

#ifndef GL_VERSION_3_0
#define GL_VERSION_3_0 1
typedef khronos_uint16_t GLhalf;
#define GL_COMPARE_REF_TO_TEXTURE         0x884E
#define GL_CLIP_DISTANCE0                 0x3000
#define GL_CLIP_DISTANCE1                 0x3001
#define GL_CLIP_DISTANCE2                 0x3002
#define GL_CLIP_DISTANCE3                 0x3003
#define GL_CLIP_DISTANCE4                 0x3004
#define GL_CLIP_DISTANCE5                 0x3005
#define GL_CLIP_DISTANCE6                 0x3006
#define GL_CLIP_DISTANCE7                 0x3007
#define GL_MAX_CLIP_DISTANCES             0x0D32
#define GL_MAJOR_VERSION                  0x821B
#define GL_MINOR_VERSION                  0x821C
#define GL_NUM_EXTENSIONS                 0x821D
#define GL_CONTEXT_FLAGS                  0x821E
#define GL_COMPRESSED_RED                 0x8225
#define GL_COMPRESSED_RG                  0x8226
#define GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT 0x00000001
#define GL_RGBA32F                        0x8814
#define GL_RGB32F                         0x8815
#define GL_RGBA16F                        0x881A
#define GL_RGB16F                         0x881B
#define GL_VERTEX_ATTRIB_ARRAY_INTEGER    0x88FD
#define GL_MAX_ARRAY_TEXTURE_LAYERS       0x88FF
#define GL_MIN_PROGRAM_TEXEL_OFFSET       0x8904
#define GL_MAX_PROGRAM_TEXEL_OFFSET       0x8905
#define GL_CLAMP_READ_COLOR               0x891C
#define GL_FIXED_ONLY                     0x891D
#define GL_MAX_VARYING_COMPONENTS         0x8B4B
#define GL_TEXTURE_1D_ARRAY               0x8C18
#define GL_PROXY_TEXTURE_1D_ARRAY         0x8C19
#define GL_TEXTURE_2D_ARRAY               0x8C1A
#define GL_PROXY_TEXTURE_2D_ARRAY         0x8C1B
#define GL_TEXTURE_BINDING_1D_ARRAY       0x8C1C
#define GL_TEXTURE_BINDING_2D_ARRAY       0x8C1D
#define GL_R11F_G11F_B10F                 0x8C3A
#define GL_UNSIGNED_INT_10F_11F_11F_REV   0x8C3B
#define GL_RGB9_E5                        0x8C3D
#define GL_UNSIGNED_INT_5_9_9_9_REV       0x8C3E
#define GL_TEXTURE_SHARED_SIZE            0x8C3F
#define GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH 0x8C76
#define GL_TRANSFORM_FEEDBACK_BUFFER_MODE 0x8C7F
#define GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS 0x8C80
#define GL_TRANSFORM_FEEDBACK_VARYINGS    0x8C83
#define GL_TRANSFORM_FEEDBACK_BUFFER_START 0x8C84
#define GL_TRANSFORM_FEEDBACK_BUFFER_SIZE 0x8C85
#define GL_PRIMITIVES_GENERATED           0x8C87
#define GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN 0x8C88
#define GL_RASTERIZER_DISCARD             0x8C89
#define GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS 0x8C8A
#define GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS 0x8C8B
#define GL_INTERLEAVED_ATTRIBS            0x8C8C
#define GL_SEPARATE_ATTRIBS               0x8C8D
#define GL_TRANSFORM_FEEDBACK_BUFFER      0x8C8E
#define GL_TRANSFORM_FEEDBACK_BUFFER_BINDING 0x8C8F
#define GL_RGBA32UI                       0x8D70
#define GL_RGB32UI                        0x8D71
#define GL_RGBA16UI                       0x8D76
#define GL_RGB16UI                        0x8D77
#define GL_RGBA8UI                        0x8D7C
#define GL_RGB8UI                         0x8D7D
#define GL_RGBA32I                        0x8D82
#define GL_RGB32I                         0x8D83
#define GL_RGBA16I                        0x8D88
#define GL_RGB16I                         0x8D89
#define GL_RGBA8I                         0x8D8E
#define GL_RGB8I                          0x8D8F
#define GL_RED_INTEGER                    0x8D94
#define GL_GREEN_INTEGER                  0x8D95
#define GL_BLUE_INTEGER                   0x8D96
#define GL_RGB_INTEGER                    0x8D98
#define GL_RGBA_INTEGER                   0x8D99
#define GL_BGR_INTEGER                    0x8D9A
#define GL_BGRA_INTEGER                   0x8D9B
#define GL_SAMPLER_1D_ARRAY               0x8DC0
#define GL_SAMPLER_2D_ARRAY               0x8DC1
#define GL_SAMPLER_1D_ARRAY_SHADOW        0x8DC3
#define GL_SAMPLER_2D_ARRAY_SHADOW        0x8DC4
#define GL_SAMPLER_CUBE_SHADOW            0x8DC5
#define GL_UNSIGNED_INT_VEC2              0x8DC6
#define GL_UNSIGNED_INT_VEC3              0x8DC7
#define GL_UNSIGNED_INT_VEC4              0x8DC8
#define GL_INT_SAMPLER_1D                 0x8DC9
#define GL_INT_SAMPLER_2D                 0x8DCA
#define GL_INT_SAMPLER_3D                 0x8DCB
#define GL_INT_SAMPLER_CUBE               0x8DCC
#define GL_INT_SAMPLER_1D_ARRAY           0x8DCE
#define GL_INT_SAMPLER_2D_ARRAY           0x8DCF
#define GL_UNSIGNED_INT_SAMPLER_1D        0x8DD1
#define GL_UNSIGNED_INT_SAMPLER_2D        0x8DD2
#define GL_UNSIGNED_INT_SAMPLER_3D        0x8DD3
#define GL_UNSIGNED_INT_SAMPLER_CUBE      0x8DD4
#define GL_UNSIGNED_INT_SAMPLER_1D_ARRAY  0x8DD6
#define GL_UNSIGNED_INT_SAMPLER_2D_ARRAY  0x8DD7
#define GL_QUERY_WAIT                     0x8E13
#define GL_QUERY_NO_WAIT                  0x8E14
#define GL_QUERY_BY_REGION_WAIT           0x8E15
#define GL_QUERY_BY_REGION_NO_WAIT        0x8E16
#define GL_BUFFER_ACCESS_FLAGS            0x911F
#define GL_BUFFER_MAP_LENGTH              0x9120
#define GL_BUFFER_MAP_OFFSET              0x9121
#define GL_DEPTH_COMPONENT32F             0x8CAC
#define GL_DEPTH32F_STENCIL8              0x8CAD
#define GL_FLOAT_32_UNSIGNED_INT_24_8_REV 0x8DAD
#define GL_INVALID_FRAMEBUFFER_OPERATION  0x0506
#define GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING 0x8210
#define GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE 0x8211
#define GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE 0x8212
#define GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE 0x8213
#define GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE 0x8214
#define GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE 0x8215
#define GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE 0x8216
#define GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE 0x8217
#define GL_FRAMEBUFFER_DEFAULT            0x8218
#define GL_FRAMEBUFFER_UNDEFINED          0x8219
#define GL_DEPTH_STENCIL_ATTACHMENT       0x821A
#define GL_MAX_RENDERBUFFER_SIZE          0x84E8
#define GL_DEPTH_STENCIL                  0x84F9
#define GL_UNSIGNED_INT_24_8              0x84FA
#define GL_DEPTH24_STENCIL8               0x88F0
#define GL_TEXTURE_STENCIL_SIZE           0x88F1
#define GL_TEXTURE_RED_TYPE               0x8C10
#define GL_TEXTURE_GREEN_TYPE             0x8C11
#define GL_TEXTURE_BLUE_TYPE              0x8C12
#define GL_TEXTURE_ALPHA_TYPE             0x8C13
#define GL_TEXTURE_DEPTH_TYPE             0x8C16
#define GL_UNSIGNED_NORMALIZED            0x8C17
#define GL_FRAMEBUFFER_BINDING            0x8CA6
#define GL_DRAW_FRAMEBUFFER_BINDING       0x8CA6
#define GL_RENDERBUFFER_BINDING           0x8CA7
#define GL_READ_FRAMEBUFFER               0x8CA8
#define GL_DRAW_FRAMEBUFFER               0x8CA9
#define GL_READ_FRAMEBUFFER_BINDING       0x8CAA
#define GL_RENDERBUFFER_SAMPLES           0x8CAB
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE 0x8CD0
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME 0x8CD1
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL 0x8CD2
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE 0x8CD3
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER 0x8CD4
#define GL_FRAMEBUFFER_COMPLETE           0x8CD5
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT 0x8CD6
#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT 0x8CD7
#define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER 0x8CDB
#define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER 0x8CDC
#define GL_FRAMEBUFFER_UNSUPPORTED        0x8CDD
#define GL_MAX_COLOR_ATTACHMENTS          0x8CDF
#define GL_COLOR_ATTACHMENT0              0x8CE0
#define GL_COLOR_ATTACHMENT1              0x8CE1
#define GL_COLOR_ATTACHMENT2              0x8CE2
#define GL_COLOR_ATTACHMENT3              0x8CE3
#define GL_COLOR_ATTACHMENT4              0x8CE4
#define GL_COLOR_ATTACHMENT5              0x8CE5
#define GL_COLOR_ATTACHMENT6              0x8CE6
#define GL_COLOR_ATTACHMENT7              0x8CE7
#define GL_COLOR_ATTACHMENT8              0x8CE8
#define GL_COLOR_ATTACHMENT9              0x8CE9
#define GL_COLOR_ATTACHMENT10             0x8CEA
#define GL_COLOR_ATTACHMENT11             0x8CEB
#define GL_COLOR_ATTACHMENT12             0x8CEC
#define GL_COLOR_ATTACHMENT13             0x8CED
#define GL_COLOR_ATTACHMENT14             0x8CEE
#define GL_COLOR_ATTACHMENT15             0x8CEF
#define GL_COLOR_ATTACHMENT16             0x8CF0
#define GL_COLOR_ATTACHMENT17             0x8CF1
#define GL_COLOR_ATTACHMENT18             0x8CF2
#define GL_COLOR_ATTACHMENT19             0x8CF3
#define GL_COLOR_ATTACHMENT20             0x8CF4
#define GL_COLOR_ATTACHMENT21             0x8CF5
#define GL_COLOR_ATTACHMENT22             0x8CF6
#define GL_COLOR_ATTACHMENT23             0x8CF7
#define GL_COLOR_ATTACHMENT24             0x8CF8
#define GL_COLOR_ATTACHMENT25             0x8CF9
#define GL_COLOR_ATTACHMENT26             0x8CFA
#define GL_COLOR_ATTACHMENT27             0x8CFB
#define GL_COLOR_ATTACHMENT28             0x8CFC
#define GL_COLOR_ATTACHMENT29             0x8CFD
#define GL_COLOR_ATTACHMENT30             0x8CFE
#define GL_COLOR_ATTACHMENT31             0x8CFF
#define GL_DEPTH_ATTACHMENT               0x8D00
#define GL_STENCIL_ATTACHMENT             0x8D20
#define GL_FRAMEBUFFER                    0x8D40
#define GL_RENDERBUFFER                   0x8D41
#define GL_RENDERBUFFER_WIDTH             0x8D42
#define GL_RENDERBUFFER_HEIGHT            0x8D43
#define GL_RENDERBUFFER_INTERNAL_FORMAT   0x8D44
#define GL_STENCIL_INDEX1                 0x8D46
#define GL_STENCIL_INDEX4                 0x8D47
#define GL_STENCIL_INDEX8                 0x8D48
#define GL_STENCIL_INDEX16                0x8D49
#define GL_RENDERBUFFER_RED_SIZE          0x8D50
#define GL_RENDERBUFFER_GREEN_SIZE        0x8D51
#define GL_RENDERBUFFER_BLUE_SIZE         0x8D52
#define GL_RENDERBUFFER_ALPHA_SIZE        0x8D53
#define GL_RENDERBUFFER_DEPTH_SIZE        0x8D54
#define GL_RENDERBUFFER_STENCIL_SIZE      0x8D55
#define GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE 0x8D56
#define GL_MAX_SAMPLES                    0x8D57
#define GL_FRAMEBUFFER_SRGB               0x8DB9
#define GL_HALF_FLOAT                     0x140B
#define GL_MAP_READ_BIT                   0x0001
#define GL_MAP_WRITE_BIT                  0x0002
#define GL_MAP_INVALIDATE_RANGE_BIT       0x0004
#define GL_MAP_INVALIDATE_BUFFER_BIT      0x0008
#define GL_MAP_FLUSH_EXPLICIT_BIT         0x0010
#define GL_MAP_UNSYNCHRONIZED_BIT         0x0020
#define GL_COMPRESSED_RED_RGTC1           0x8DBB
#define GL_COMPRESSED_SIGNED_RED_RGTC1    0x8DBC
#define GL_COMPRESSED_RG_RGTC2            0x8DBD
#define GL_COMPRESSED_SIGNED_RG_RGTC2     0x8DBE
#define GL_RG                             0x8227
#define GL_RG_INTEGER                     0x8228
#define GL_R8                             0x8229
#define GL_R16                            0x822A
#define GL_RG8                            0x822B
#define GL_RG16                           0x822C
#define GL_R16F                           0x822D
#define GL_R32F                           0x822E
#define GL_RG16F                          0x822F
#define GL_RG32F                          0x8230
#define GL_R8I                            0x8231
#define GL_R8UI                           0x8232
#define GL_R16I                           0x8233
#define GL_R16UI                          0x8234
#define GL_R32I                           0x8235
#define GL_R32UI                          0x8236
#define GL_RG8I                           0x8237
#define GL_RG8UI                          0x8238
#define GL_RG16I                          0x8239
#define GL_RG16UI                         0x823A
#define GL_RG32I                          0x823B
#define GL_RG32UI                         0x823C
#define GL_VERTEX_ARRAY_BINDING           0x85B5
#ifdef GL_GLEXT_PROTOTYPES
#endif
#endif /* GL_VERSION_3_0 */

#ifndef GL_VERSION_3_1
#define GL_VERSION_3_1 1
#define GL_SAMPLER_2D_RECT                0x8B63
#define GL_SAMPLER_2D_RECT_SHADOW         0x8B64
#define GL_SAMPLER_BUFFER                 0x8DC2
#define GL_INT_SAMPLER_2D_RECT            0x8DCD
#define GL_INT_SAMPLER_BUFFER             0x8DD0
#define GL_UNSIGNED_INT_SAMPLER_2D_RECT   0x8DD5
#define GL_UNSIGNED_INT_SAMPLER_BUFFER    0x8DD8
#define GL_TEXTURE_BUFFER                 0x8C2A
#define GL_MAX_TEXTURE_BUFFER_SIZE        0x8C2B
#define GL_TEXTURE_BINDING_BUFFER         0x8C2C
#define GL_TEXTURE_BUFFER_DATA_STORE_BINDING 0x8C2D
#define GL_TEXTURE_RECTANGLE              0x84F5
#define GL_TEXTURE_BINDING_RECTANGLE      0x84F6
#define GL_PROXY_TEXTURE_RECTANGLE        0x84F7
#define GL_MAX_RECTANGLE_TEXTURE_SIZE     0x84F8
#define GL_R8_SNORM                       0x8F94
#define GL_RG8_SNORM                      0x8F95
#define GL_RGB8_SNORM                     0x8F96
#define GL_RGBA8_SNORM                    0x8F97
#define GL_R16_SNORM                      0x8F98
#define GL_RG16_SNORM                     0x8F99
#define GL_RGB16_SNORM                    0x8F9A
#define GL_RGBA16_SNORM                   0x8F9B
#define GL_SIGNED_NORMALIZED              0x8F9C
#define GL_PRIMITIVE_RESTART              0x8F9D
#define GL_PRIMITIVE_RESTART_INDEX        0x8F9E
#define GL_COPY_READ_BUFFER               0x8F36
#define GL_COPY_WRITE_BUFFER              0x8F37
#define GL_UNIFORM_BUFFER                 0x8A11
#define GL_UNIFORM_BUFFER_BINDING         0x8A28
#define GL_UNIFORM_BUFFER_START           0x8A29
#define GL_UNIFORM_BUFFER_SIZE            0x8A2A
#define GL_MAX_VERTEX_UNIFORM_BLOCKS      0x8A2B
#define GL_MAX_GEOMETRY_UNIFORM_BLOCKS    0x8A2C
#define GL_MAX_FRAGMENT_UNIFORM_BLOCKS    0x8A2D
#define GL_MAX_COMBINED_UNIFORM_BLOCKS    0x8A2E
#define GL_MAX_UNIFORM_BUFFER_BINDINGS    0x8A2F
#define GL_MAX_UNIFORM_BLOCK_SIZE         0x8A30
#define GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS 0x8A31
#define GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS 0x8A32
#define GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS 0x8A33
#define GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT 0x8A34
#define GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH 0x8A35
#define GL_ACTIVE_UNIFORM_BLOCKS          0x8A36
#define GL_UNIFORM_TYPE                   0x8A37
#define GL_UNIFORM_SIZE                   0x8A38
#define GL_UNIFORM_NAME_LENGTH            0x8A39
#define GL_UNIFORM_BLOCK_INDEX            0x8A3A
#define GL_UNIFORM_OFFSET                 0x8A3B
#define GL_UNIFORM_ARRAY_STRIDE           0x8A3C
#define GL_UNIFORM_MATRIX_STRIDE          0x8A3D
#define GL_UNIFORM_IS_ROW_MAJOR           0x8A3E
#define GL_UNIFORM_BLOCK_BINDING          0x8A3F
#define GL_UNIFORM_BLOCK_DATA_SIZE        0x8A40
#define GL_UNIFORM_BLOCK_NAME_LENGTH      0x8A41
#define GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS  0x8A42
#define GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES 0x8A43
#define GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER 0x8A44
#define GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER 0x8A45
#define GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER 0x8A46
#define GL_INVALID_INDEX                  0xFFFFFFFFu
#ifdef GL_GLEXT_PROTOTYPES
#endif
#endif /* GL_VERSION_3_1 */

#ifndef GL_VERSION_3_2
#define GL_VERSION_3_2 1
typedef struct __GLsync *GLsync;
typedef khronos_uint64_t GLuint64;
typedef khronos_int64_t GLint64;
#define GL_CONTEXT_CORE_PROFILE_BIT       0x00000001
#define GL_CONTEXT_COMPATIBILITY_PROFILE_BIT 0x00000002
#define GL_LINES_ADJACENCY                0x000A
#define GL_LINE_STRIP_ADJACENCY           0x000B
#define GL_TRIANGLES_ADJACENCY            0x000C
#define GL_TRIANGLE_STRIP_ADJACENCY       0x000D
#define GL_PROGRAM_POINT_SIZE             0x8642
#define GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS 0x8C29
#define GL_FRAMEBUFFER_ATTACHMENT_LAYERED 0x8DA7
#define GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS 0x8DA8
#define GL_GEOMETRY_SHADER                0x8DD9
#define GL_GEOMETRY_VERTICES_OUT          0x8916
#define GL_GEOMETRY_INPUT_TYPE            0x8917
#define GL_GEOMETRY_OUTPUT_TYPE           0x8918
#define GL_MAX_GEOMETRY_UNIFORM_COMPONENTS 0x8DDF
#define GL_MAX_GEOMETRY_OUTPUT_VERTICES   0x8DE0
#define GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS 0x8DE1
#define GL_MAX_VERTEX_OUTPUT_COMPONENTS   0x9122
#define GL_MAX_GEOMETRY_INPUT_COMPONENTS  0x9123
#define GL_MAX_GEOMETRY_OUTPUT_COMPONENTS 0x9124
#define GL_MAX_FRAGMENT_INPUT_COMPONENTS  0x9125
#define GL_CONTEXT_PROFILE_MASK           0x9126
#define GL_DEPTH_CLAMP                    0x864F
#define GL_QUADS_FOLLOW_PROVOKING_VERTEX_CONVENTION 0x8E4C
#define GL_FIRST_VERTEX_CONVENTION        0x8E4D
#define GL_LAST_VERTEX_CONVENTION         0x8E4E
#define GL_PROVOKING_VERTEX               0x8E4F
#define GL_TEXTURE_CUBE_MAP_SEAMLESS      0x884F
#define GL_MAX_SERVER_WAIT_TIMEOUT        0x9111
#define GL_OBJECT_TYPE                    0x9112
#define GL_SYNC_CONDITION                 0x9113
#define GL_SYNC_STATUS                    0x9114
#define GL_SYNC_FLAGS                     0x9115
#define GL_SYNC_FENCE                     0x9116
#define GL_SYNC_GPU_COMMANDS_COMPLETE     0x9117
#define GL_UNSIGNALED                     0x9118
#define GL_SIGNALED                       0x9119
#define GL_ALREADY_SIGNALED               0x911A
#define GL_TIMEOUT_EXPIRED                0x911B
#define GL_CONDITION_SATISFIED            0x911C
#define GL_WAIT_FAILED                    0x911D
#define GL_TIMEOUT_IGNORED                0xFFFFFFFFFFFFFFFFull
#define GL_SYNC_FLUSH_COMMANDS_BIT        0x00000001
#define GL_SAMPLE_POSITION                0x8E50
#define GL_SAMPLE_MASK                    0x8E51
#define GL_SAMPLE_MASK_VALUE              0x8E52
#define GL_MAX_SAMPLE_MASK_WORDS          0x8E59
#define GL_TEXTURE_2D_MULTISAMPLE         0x9100
#define GL_PROXY_TEXTURE_2D_MULTISAMPLE   0x9101
#define GL_TEXTURE_2D_MULTISAMPLE_ARRAY   0x9102
#define GL_PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY 0x9103
#define GL_TEXTURE_BINDING_2D_MULTISAMPLE 0x9104
#define GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY 0x9105
#define GL_TEXTURE_SAMPLES                0x9106
#define GL_TEXTURE_FIXED_SAMPLE_LOCATIONS 0x9107
#define GL_SAMPLER_2D_MULTISAMPLE         0x9108
#define GL_INT_SAMPLER_2D_MULTISAMPLE     0x9109
#define GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE 0x910A
#define GL_SAMPLER_2D_MULTISAMPLE_ARRAY   0x910B
#define GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY 0x910C
#define GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY 0x910D
#define GL_MAX_COLOR_TEXTURE_SAMPLES      0x910E
#define GL_MAX_DEPTH_TEXTURE_SAMPLES      0x910F
#define GL_MAX_INTEGER_SAMPLES            0x9110
#ifdef GL_GLEXT_PROTOTYPES
#endif
#endif /* GL_VERSION_3_2 */

#ifndef GL_VERSION_3_3
#define GL_VERSION_3_3 1
#define GL_VERTEX_ATTRIB_ARRAY_DIVISOR    0x88FE
#define GL_SRC1_COLOR                     0x88F9
#define GL_ONE_MINUS_SRC1_COLOR           0x88FA
#define GL_ONE_MINUS_SRC1_ALPHA           0x88FB
#define GL_MAX_DUAL_SOURCE_DRAW_BUFFERS   0x88FC
#define GL_ANY_SAMPLES_PASSED             0x8C2F
#define GL_SAMPLER_BINDING                0x8919
#define GL_RGB10_A2UI                     0x906F
#define GL_TEXTURE_SWIZZLE_R              0x8E42
#define GL_TEXTURE_SWIZZLE_G              0x8E43
#define GL_TEXTURE_SWIZZLE_B              0x8E44
#define GL_TEXTURE_SWIZZLE_A              0x8E45
#define GL_TEXTURE_SWIZZLE_RGBA           0x8E46
#define GL_TIME_ELAPSED                   0x88BF
#define GL_TIMESTAMP                      0x8E28
#define GL_INT_2_10_10_10_REV             0x8D9F
#ifdef GL_GLEXT_PROTOTYPES
#endif
#endif /* GL_VERSION_3_3 */

#ifndef GL_VERSION_4_0
#define GL_VERSION_4_0 1
#define GL_SAMPLE_SHADING                 0x8C36
#define GL_MIN_SAMPLE_SHADING_VALUE       0x8C37
#define GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET 0x8E5E
#define GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET 0x8E5F
#define GL_TEXTURE_CUBE_MAP_ARRAY         0x9009
#define GL_TEXTURE_BINDING_CUBE_MAP_ARRAY 0x900A
#define GL_PROXY_TEXTURE_CUBE_MAP_ARRAY   0x900B
#define GL_SAMPLER_CUBE_MAP_ARRAY         0x900C
#define GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW  0x900D
#define GL_INT_SAMPLER_CUBE_MAP_ARRAY     0x900E
#define GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY 0x900F
#define GL_DRAW_INDIRECT_BUFFER           0x8F3F
#define GL_DRAW_INDIRECT_BUFFER_BINDING   0x8F43
#define GL_GEOMETRY_SHADER_INVOCATIONS    0x887F
#define GL_MAX_GEOMETRY_SHADER_INVOCATIONS 0x8E5A
#define GL_MIN_FRAGMENT_INTERPOLATION_OFFSET 0x8E5B
#define GL_MAX_FRAGMENT_INTERPOLATION_OFFSET 0x8E5C
#define GL_FRAGMENT_INTERPOLATION_OFFSET_BITS 0x8E5D
#define GL_MAX_VERTEX_STREAMS             0x8E71
#define GL_DOUBLE_VEC2                    0x8FFC
#define GL_DOUBLE_VEC3                    0x8FFD
#define GL_DOUBLE_VEC4                    0x8FFE
#define GL_DOUBLE_MAT2                    0x8F46
#define GL_DOUBLE_MAT3                    0x8F47
#define GL_DOUBLE_MAT4                    0x8F48
#define GL_DOUBLE_MAT2x3                  0x8F49
#define GL_DOUBLE_MAT2x4                  0x8F4A
#define GL_DOUBLE_MAT3x2                  0x8F4B
#define GL_DOUBLE_MAT3x4                  0x8F4C
#define GL_DOUBLE_MAT4x2                  0x8F4D
#define GL_DOUBLE_MAT4x3                  0x8F4E
#define GL_ACTIVE_SUBROUTINES             0x8DE5
#define GL_ACTIVE_SUBROUTINE_UNIFORMS     0x8DE6
#define GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS 0x8E47
#define GL_ACTIVE_SUBROUTINE_MAX_LENGTH   0x8E48
#define GL_ACTIVE_SUBROUTINE_UNIFORM_MAX_LENGTH 0x8E49
#define GL_MAX_SUBROUTINES                0x8DE7
#define GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS 0x8DE8
#define GL_NUM_COMPATIBLE_SUBROUTINES     0x8E4A
#define GL_COMPATIBLE_SUBROUTINES         0x8E4B
#define GL_PATCHES                        0x000E
#define GL_PATCH_VERTICES                 0x8E72
#define GL_PATCH_DEFAULT_INNER_LEVEL      0x8E73
#define GL_PATCH_DEFAULT_OUTER_LEVEL      0x8E74
#define GL_TESS_CONTROL_OUTPUT_VERTICES   0x8E75
#define GL_TESS_GEN_MODE                  0x8E76
#define GL_TESS_GEN_SPACING               0x8E77
#define GL_TESS_GEN_VERTEX_ORDER          0x8E78
#define GL_TESS_GEN_POINT_MODE            0x8E79
#define GL_ISOLINES                       0x8E7A
#define GL_FRACTIONAL_ODD                 0x8E7B
#define GL_FRACTIONAL_EVEN                0x8E7C
#define GL_MAX_PATCH_VERTICES             0x8E7D
#define GL_MAX_TESS_GEN_LEVEL             0x8E7E
#define GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS 0x8E7F
#define GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS 0x8E80
#define GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS 0x8E81
#define GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS 0x8E82
#define GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS 0x8E83
#define GL_MAX_TESS_PATCH_COMPONENTS      0x8E84
#define GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS 0x8E85
#define GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS 0x8E86
#define GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS 0x8E89
#define GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS 0x8E8A
#define GL_MAX_TESS_CONTROL_INPUT_COMPONENTS 0x886C
#define GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS 0x886D
#define GL_MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS 0x8E1E
#define GL_MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS 0x8E1F
#define GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_CONTROL_SHADER 0x84F0
#define GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_EVALUATION_SHADER 0x84F1
#define GL_TESS_EVALUATION_SHADER         0x8E87
#define GL_TESS_CONTROL_SHADER            0x8E88
#define GL_TRANSFORM_FEEDBACK             0x8E22
#define GL_TRANSFORM_FEEDBACK_BUFFER_PAUSED 0x8E23
#define GL_TRANSFORM_FEEDBACK_BUFFER_ACTIVE 0x8E24
#define GL_TRANSFORM_FEEDBACK_BINDING     0x8E25
#define GL_MAX_TRANSFORM_FEEDBACK_BUFFERS 0x8E70
#ifdef GL_GLEXT_PROTOTYPES
#endif
#endif /* GL_VERSION_4_0 */


typedef void (APIENTRY  *GLDEBUGPROCARB)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);
#define GL_DEBUG_SEVERITY_HIGH            0x9146
#define GL_DEBUG_SEVERITY_MEDIUM          0x9147
#define GL_DEBUG_SEVERITY_LOW             0x9148
#define GL_DEBUG_SEVERITY_NOTIFICATION    0x826B
#define GL_DEBUG_SEVERITY_HIGH_ARB        0x9146
#define GL_DEBUG_SEVERITY_MEDIUM_ARB      0x9147
#define GL_DEBUG_SEVERITY_LOW_ARB         0x9148
#define GL_DEBUG_OUTPUT_SYNCHRONOUS       0x8242
#define GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB   0x8242
void APIENTRY glCullFace (GLenum mode);
void APIENTRY glFrontFace (GLenum mode);
void APIENTRY glHint (GLenum target, GLenum mode);
void APIENTRY glLineWidth (GLfloat width);
void APIENTRY glPointSize (GLfloat size);
void APIENTRY glPolygonMode (GLenum face, GLenum mode);
void APIENTRY glScissor (GLint x, GLint y, GLsizei width, GLsizei height);
void APIENTRY glTexParameterf (GLenum target, GLenum pname, GLfloat param);
void APIENTRY glTexParameterfv (GLenum target, GLenum pname, const GLfloat *params);
void APIENTRY glTexParameteri (GLenum target, GLenum pname, GLint param);
void APIENTRY glTexParameteriv (GLenum target, GLenum pname, const GLint *params);
void APIENTRY glTexImage1D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const void *pixels);
void APIENTRY glTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
void APIENTRY glDrawBuffer (GLenum buf);
void APIENTRY glClear (GLbitfield mask);
void APIENTRY glClearColor (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
void APIENTRY glClearStencil (GLint s);
void APIENTRY glClearDepth (GLdouble depth);
void APIENTRY glStencilMask (GLuint mask);
void APIENTRY glColorMask (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
void APIENTRY glDepthMask (GLboolean flag);
void APIENTRY glDisable (GLenum cap);
void APIENTRY glEnable (GLenum cap);
void APIENTRY glFinish (void);
void APIENTRY glFlush (void);
void APIENTRY glBlendFunc (GLenum sfactor, GLenum dfactor);
void APIENTRY glLogicOp (GLenum opcode);
void APIENTRY glStencilFunc (GLenum func, GLint ref, GLuint mask);
void APIENTRY glStencilOp (GLenum fail, GLenum zfail, GLenum zpass);
void APIENTRY glDepthFunc (GLenum func);
void APIENTRY glPixelStoref (GLenum pname, GLfloat param);
void APIENTRY glPixelStorei (GLenum pname, GLint param);
void APIENTRY glReadBuffer (GLenum src);
void APIENTRY glReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels);
void APIENTRY glGetBooleanv (GLenum pname, GLboolean *data);
void APIENTRY glGetDoublev (GLenum pname, GLdouble *data);
GLenum APIENTRY glGetError (void);
void APIENTRY glGetFloatv (GLenum pname, GLfloat *data);
void APIENTRY glGetIntegerv (GLenum pname, GLint *data);
void APIENTRY glGetTexImage (GLenum target, GLint level, GLenum format, GLenum type, void *pixels);
void APIENTRY glGetTexParameterfv (GLenum target, GLenum pname, GLfloat *params);
void APIENTRY glGetTexParameteriv (GLenum target, GLenum pname, GLint *params);
void APIENTRY glGetTexLevelParameterfv (GLenum target, GLint level, GLenum pname, GLfloat *params);
void APIENTRY glGetTexLevelParameteriv (GLenum target, GLint level, GLenum pname, GLint *params);
GLboolean APIENTRY glIsEnabled (GLenum cap);
void APIENTRY glDepthRange (GLdouble n, GLdouble f);
void APIENTRY glViewport (GLint x, GLint y, GLsizei width, GLsizei height);
void APIENTRY glDrawArrays (GLenum mode, GLint first, GLsizei count);
void APIENTRY glDrawElements (GLenum mode, GLsizei count, GLenum type, const void *indices);
void APIENTRY glGetPointerv (GLenum pname, void **params);
void APIENTRY glPolygonOffset (GLfloat factor, GLfloat units);
void APIENTRY glCopyTexImage1D (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border);
void APIENTRY glCopyTexImage2D (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
void APIENTRY glCopyTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
void APIENTRY glCopyTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
void APIENTRY glTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void *pixels);
void APIENTRY glTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
void APIENTRY glBindTexture (GLenum target, GLuint texture);
void APIENTRY glDeleteTextures (GLsizei n, const GLuint *textures);
void APIENTRY glGenTextures (GLsizei n, GLuint *textures);
GLboolean APIENTRY glIsTexture (GLuint texture);
typedef struct gl_api gl_api;
struct gl_api {
void APIENTRY (*glCullFace) (GLenum mode);
void APIENTRY (*glFrontFace) (GLenum mode);
void APIENTRY (*glHint) (GLenum target, GLenum mode);
void APIENTRY (*glLineWidth) (GLfloat width);
void APIENTRY (*glPointSize) (GLfloat size);
void APIENTRY (*glPolygonMode) (GLenum face, GLenum mode);
void APIENTRY (*glScissor) (GLint x, GLint y, GLsizei width, GLsizei height);
void APIENTRY (*glTexParameterf) (GLenum target, GLenum pname, GLfloat param);
void APIENTRY (*glTexParameterfv) (GLenum target, GLenum pname, const GLfloat *params);
void APIENTRY (*glTexParameteri) (GLenum target, GLenum pname, GLint param);
void APIENTRY (*glTexParameteriv) (GLenum target, GLenum pname, const GLint *params);
void APIENTRY (*glTexImage1D) (GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const void *pixels);
void APIENTRY (*glTexImage2D) (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
void APIENTRY (*glDrawBuffer) (GLenum buf);
void APIENTRY (*glClear) (GLbitfield mask);
void APIENTRY (*glClearColor) (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
void APIENTRY (*glClearStencil) (GLint s);
void APIENTRY (*glClearDepth) (GLdouble depth);
void APIENTRY (*glStencilMask) (GLuint mask);
void APIENTRY (*glColorMask) (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
void APIENTRY (*glDepthMask) (GLboolean flag);
void APIENTRY (*glDisable) (GLenum cap);
void APIENTRY (*glEnable) (GLenum cap);
void APIENTRY (*glFinish) (void);
void APIENTRY (*glFlush) (void);
void APIENTRY (*glBlendFunc) (GLenum sfactor, GLenum dfactor);
void APIENTRY (*glLogicOp) (GLenum opcode);
void APIENTRY (*glStencilFunc) (GLenum func, GLint ref, GLuint mask);
void APIENTRY (*glStencilOp) (GLenum fail, GLenum zfail, GLenum zpass);
void APIENTRY (*glDepthFunc) (GLenum func);
void APIENTRY (*glPixelStoref) (GLenum pname, GLfloat param);
void APIENTRY (*glPixelStorei) (GLenum pname, GLint param);
void APIENTRY (*glReadBuffer) (GLenum src);
void APIENTRY (*glReadPixels) (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels);
void APIENTRY (*glGetBooleanv) (GLenum pname, GLboolean *data);
void APIENTRY (*glGetDoublev) (GLenum pname, GLdouble *data);
GLenum APIENTRY (*glGetError) (void);
void APIENTRY (*glGetFloatv) (GLenum pname, GLfloat *data);
void APIENTRY (*glGetIntegerv) (GLenum pname, GLint *data);
const GLubyte *APIENTRY (*glGetString) (GLenum name);
void APIENTRY (*glGetTexImage) (GLenum target, GLint level, GLenum format, GLenum type, void *pixels);
void APIENTRY (*glGetTexParameterfv) (GLenum target, GLenum pname, GLfloat *params);
void APIENTRY (*glGetTexParameteriv) (GLenum target, GLenum pname, GLint *params);
void APIENTRY (*glGetTexLevelParameterfv) (GLenum target, GLint level, GLenum pname, GLfloat *params);
void APIENTRY (*glGetTexLevelParameteriv) (GLenum target, GLint level, GLenum pname, GLint *params);
GLboolean APIENTRY (*glIsEnabled) (GLenum cap);
void APIENTRY (*glDepthRange) (GLdouble n, GLdouble f);
void APIENTRY (*glViewport) (GLint x, GLint y, GLsizei width, GLsizei height);
void APIENTRY (*glDrawArrays) (GLenum mode, GLint first, GLsizei count);
void APIENTRY (*glDrawElements) (GLenum mode, GLsizei count, GLenum type, const void *indices);
void APIENTRY (*glGetPointerv) (GLenum pname, void **params);
void APIENTRY (*glPolygonOffset) (GLfloat factor, GLfloat units);
void APIENTRY (*glCopyTexImage1D) (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border);
void APIENTRY (*glCopyTexImage2D) (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
void APIENTRY (*glCopyTexSubImage1D) (GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
void APIENTRY (*glCopyTexSubImage2D) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
void APIENTRY (*glTexSubImage1D) (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void *pixels);
void APIENTRY (*glTexSubImage2D) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
void APIENTRY (*glBindTexture) (GLenum target, GLuint texture);
void APIENTRY (*glDeleteTextures) (GLsizei n, const GLuint *textures);
void APIENTRY (*glGenTextures) (GLsizei n, GLuint *textures);
GLboolean APIENTRY (*glIsTexture) (GLuint texture);
void APIENTRY (*glDrawRangeElements) (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices);
void APIENTRY (*glTexImage3D) (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels);
void APIENTRY (*glTexSubImage3D) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels);
void APIENTRY (*glCopyTexSubImage3D) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
void APIENTRY (*glActiveTexture) (GLenum texture);
void APIENTRY (*glSampleCoverage) (GLfloat value, GLboolean invert);
void APIENTRY (*glCompressedTexImage3D) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data);
void APIENTRY (*glCompressedTexImage2D) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data);
void APIENTRY (*glCompressedTexImage1D) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const void *data);
void APIENTRY (*glCompressedTexSubImage3D) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data);
void APIENTRY (*glCompressedTexSubImage2D) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data);
void APIENTRY (*glCompressedTexSubImage1D) (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void *data);
void APIENTRY (*glGetCompressedTexImage) (GLenum target, GLint level, void *img);
void APIENTRY (*glBlendFuncSeparate) (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
void APIENTRY (*glMultiDrawArrays) (GLenum mode, const GLint *first, const GLsizei *count, GLsizei drawcount);
void APIENTRY (*glMultiDrawElements) (GLenum mode, const GLsizei *count, GLenum type, const void *const*indices, GLsizei drawcount);
void APIENTRY (*glPointParameterf) (GLenum pname, GLfloat param);
void APIENTRY (*glPointParameterfv) (GLenum pname, const GLfloat *params);
void APIENTRY (*glPointParameteri) (GLenum pname, GLint param);
void APIENTRY (*glPointParameteriv) (GLenum pname, const GLint *params);
void APIENTRY (*glBlendColor) (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
void APIENTRY (*glBlendEquation) (GLenum mode);
void APIENTRY (*glGenQueries) (GLsizei n, GLuint *ids);
void APIENTRY (*glDeleteQueries) (GLsizei n, const GLuint *ids);
GLboolean APIENTRY (*glIsQuery) (GLuint id);
void APIENTRY (*glBeginQuery) (GLenum target, GLuint id);
void APIENTRY (*glEndQuery) (GLenum target);
void APIENTRY (*glGetQueryiv) (GLenum target, GLenum pname, GLint *params);
void APIENTRY (*glGetQueryObjectiv) (GLuint id, GLenum pname, GLint *params);
void APIENTRY (*glGetQueryObjectuiv) (GLuint id, GLenum pname, GLuint *params);
void APIENTRY (*glBindBuffer) (GLenum target, GLuint buffer);
void APIENTRY (*glDeleteBuffers) (GLsizei n, const GLuint *buffers);
void APIENTRY (*glGenBuffers) (GLsizei n, GLuint *buffers);
GLboolean APIENTRY (*glIsBuffer) (GLuint buffer);
void APIENTRY (*glBufferData) (GLenum target, GLsizeiptr size, const void *data, GLenum usage);
void APIENTRY (*glBufferSubData) (GLenum target, GLintptr offset, GLsizeiptr size, const void *data);
void APIENTRY (*glGetBufferSubData) (GLenum target, GLintptr offset, GLsizeiptr size, void *data);
void *APIENTRY (*glMapBuffer) (GLenum target, GLenum access);
GLboolean APIENTRY (*glUnmapBuffer) (GLenum target);
void APIENTRY (*glGetBufferParameteriv) (GLenum target, GLenum pname, GLint *params);
void APIENTRY (*glGetBufferPointerv) (GLenum target, GLenum pname, void **params);
void APIENTRY (*glBlendEquationSeparate) (GLenum modeRGB, GLenum modeAlpha);
void APIENTRY (*glDrawBuffers) (GLsizei n, const GLenum *bufs);
void APIENTRY (*glStencilOpSeparate) (GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);
void APIENTRY (*glStencilFuncSeparate) (GLenum face, GLenum func, GLint ref, GLuint mask);
void APIENTRY (*glStencilMaskSeparate) (GLenum face, GLuint mask);
void APIENTRY (*glAttachShader) (GLuint program, GLuint shader);
void APIENTRY (*glBindAttribLocation) (GLuint program, GLuint index, const GLchar *name);
void APIENTRY (*glCompileShader) (GLuint shader);
GLuint APIENTRY (*glCreateProgram) (void);
GLuint APIENTRY (*glCreateShader) (GLenum type);
void APIENTRY (*glDeleteProgram) (GLuint program);
void APIENTRY (*glDeleteShader) (GLuint shader);
void APIENTRY (*glDetachShader) (GLuint program, GLuint shader);
void APIENTRY (*glDisableVertexAttribArray) (GLuint index);
void APIENTRY (*glEnableVertexAttribArray) (GLuint index);
void APIENTRY (*glGetActiveAttrib) (GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
void APIENTRY (*glGetActiveUniform) (GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
void APIENTRY (*glGetAttachedShaders) (GLuint program, GLsizei maxCount, GLsizei *count, GLuint *shaders);
GLint APIENTRY (*glGetAttribLocation) (GLuint program, const GLchar *name);
void APIENTRY (*glGetProgramiv) (GLuint program, GLenum pname, GLint *params);
void APIENTRY (*glGetProgramInfoLog) (GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
void APIENTRY (*glGetShaderiv) (GLuint shader, GLenum pname, GLint *params);
void APIENTRY (*glGetShaderInfoLog) (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
void APIENTRY (*glGetShaderSource) (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *source);
GLint APIENTRY (*glGetUniformLocation) (GLuint program, const GLchar *name);
void APIENTRY (*glGetUniformfv) (GLuint program, GLint location, GLfloat *params);
void APIENTRY (*glGetUniformiv) (GLuint program, GLint location, GLint *params);
void APIENTRY (*glGetVertexAttribdv) (GLuint index, GLenum pname, GLdouble *params);
void APIENTRY (*glGetVertexAttribfv) (GLuint index, GLenum pname, GLfloat *params);
void APIENTRY (*glGetVertexAttribiv) (GLuint index, GLenum pname, GLint *params);
void APIENTRY (*glGetVertexAttribPointerv) (GLuint index, GLenum pname, void **pointer);
GLboolean APIENTRY (*glIsProgram) (GLuint program);
GLboolean APIENTRY (*glIsShader) (GLuint shader);
void APIENTRY (*glLinkProgram) (GLuint program);
void APIENTRY (*glShaderSource) (GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
void APIENTRY (*glUseProgram) (GLuint program);
void APIENTRY (*glUniform1f) (GLint location, GLfloat v0);
void APIENTRY (*glUniform2f) (GLint location, GLfloat v0, GLfloat v1);
void APIENTRY (*glUniform3f) (GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
void APIENTRY (*glUniform4f) (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
void APIENTRY (*glUniform1i) (GLint location, GLint v0);
void APIENTRY (*glUniform2i) (GLint location, GLint v0, GLint v1);
void APIENTRY (*glUniform3i) (GLint location, GLint v0, GLint v1, GLint v2);
void APIENTRY (*glUniform4i) (GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
void APIENTRY (*glUniform1fv) (GLint location, GLsizei count, const GLfloat *value);
void APIENTRY (*glUniform2fv) (GLint location, GLsizei count, const GLfloat *value);
void APIENTRY (*glUniform3fv) (GLint location, GLsizei count, const GLfloat *value);
void APIENTRY (*glUniform4fv) (GLint location, GLsizei count, const GLfloat *value);
void APIENTRY (*glUniform1iv) (GLint location, GLsizei count, const GLint *value);
void APIENTRY (*glUniform2iv) (GLint location, GLsizei count, const GLint *value);
void APIENTRY (*glUniform3iv) (GLint location, GLsizei count, const GLint *value);
void APIENTRY (*glUniform4iv) (GLint location, GLsizei count, const GLint *value);
void APIENTRY (*glUniformMatrix2fv) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
void APIENTRY (*glUniformMatrix3fv) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
void APIENTRY (*glUniformMatrix4fv) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
void APIENTRY (*glValidateProgram) (GLuint program);
void APIENTRY (*glVertexAttrib1d) (GLuint index, GLdouble x);
void APIENTRY (*glVertexAttrib1dv) (GLuint index, const GLdouble *v);
void APIENTRY (*glVertexAttrib1f) (GLuint index, GLfloat x);
void APIENTRY (*glVertexAttrib1fv) (GLuint index, const GLfloat *v);
void APIENTRY (*glVertexAttrib1s) (GLuint index, GLshort x);
void APIENTRY (*glVertexAttrib1sv) (GLuint index, const GLshort *v);
void APIENTRY (*glVertexAttrib2d) (GLuint index, GLdouble x, GLdouble y);
void APIENTRY (*glVertexAttrib2dv) (GLuint index, const GLdouble *v);
void APIENTRY (*glVertexAttrib2f) (GLuint index, GLfloat x, GLfloat y);
void APIENTRY (*glVertexAttrib2fv) (GLuint index, const GLfloat *v);
void APIENTRY (*glVertexAttrib2s) (GLuint index, GLshort x, GLshort y);
void APIENTRY (*glVertexAttrib2sv) (GLuint index, const GLshort *v);
void APIENTRY (*glVertexAttrib3d) (GLuint index, GLdouble x, GLdouble y, GLdouble z);
void APIENTRY (*glVertexAttrib3dv) (GLuint index, const GLdouble *v);
void APIENTRY (*glVertexAttrib3f) (GLuint index, GLfloat x, GLfloat y, GLfloat z);
void APIENTRY (*glVertexAttrib3fv) (GLuint index, const GLfloat *v);
void APIENTRY (*glVertexAttrib3s) (GLuint index, GLshort x, GLshort y, GLshort z);
void APIENTRY (*glVertexAttrib3sv) (GLuint index, const GLshort *v);
void APIENTRY (*glVertexAttrib4Nbv) (GLuint index, const GLbyte *v);
void APIENTRY (*glVertexAttrib4Niv) (GLuint index, const GLint *v);
void APIENTRY (*glVertexAttrib4Nsv) (GLuint index, const GLshort *v);
void APIENTRY (*glVertexAttrib4Nub) (GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w);
void APIENTRY (*glVertexAttrib4Nubv) (GLuint index, const GLubyte *v);
void APIENTRY (*glVertexAttrib4Nuiv) (GLuint index, const GLuint *v);
void APIENTRY (*glVertexAttrib4Nusv) (GLuint index, const GLushort *v);
void APIENTRY (*glVertexAttrib4bv) (GLuint index, const GLbyte *v);
void APIENTRY (*glVertexAttrib4d) (GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
void APIENTRY (*glVertexAttrib4dv) (GLuint index, const GLdouble *v);
void APIENTRY (*glVertexAttrib4f) (GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
void APIENTRY (*glVertexAttrib4fv) (GLuint index, const GLfloat *v);
void APIENTRY (*glVertexAttrib4iv) (GLuint index, const GLint *v);
void APIENTRY (*glVertexAttrib4s) (GLuint index, GLshort x, GLshort y, GLshort z, GLshort w);
void APIENTRY (*glVertexAttrib4sv) (GLuint index, const GLshort *v);
void APIENTRY (*glVertexAttrib4ubv) (GLuint index, const GLubyte *v);
void APIENTRY (*glVertexAttrib4uiv) (GLuint index, const GLuint *v);
void APIENTRY (*glVertexAttrib4usv) (GLuint index, const GLushort *v);
void APIENTRY (*glVertexAttribPointer) (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
void APIENTRY (*glUniformMatrix2x3fv) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
void APIENTRY (*glUniformMatrix3x2fv) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
void APIENTRY (*glUniformMatrix2x4fv) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
void APIENTRY (*glUniformMatrix4x2fv) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
void APIENTRY (*glUniformMatrix3x4fv) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
void APIENTRY (*glUniformMatrix4x3fv) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
void APIENTRY (*glColorMaski) (GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a);
void APIENTRY (*glGetBooleani_v) (GLenum target, GLuint index, GLboolean *data);
void APIENTRY (*glGetIntegeri_v) (GLenum target, GLuint index, GLint *data);
void APIENTRY (*glEnablei) (GLenum target, GLuint index);
void APIENTRY (*glDisablei) (GLenum target, GLuint index);
GLboolean APIENTRY (*glIsEnabledi) (GLenum target, GLuint index);
void APIENTRY (*glBeginTransformFeedback) (GLenum primitiveMode);
void APIENTRY (*glEndTransformFeedback) (void);
void APIENTRY (*glBindBufferRange) (GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
void APIENTRY (*glBindBufferBase) (GLenum target, GLuint index, GLuint buffer);
void APIENTRY (*glTransformFeedbackVaryings) (GLuint program, GLsizei count, const GLchar *const*varyings, GLenum bufferMode);
void APIENTRY (*glGetTransformFeedbackVarying) (GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLsizei *size, GLenum *type, GLchar *name);
void APIENTRY (*glClampColor) (GLenum target, GLenum clamp);
void APIENTRY (*glBeginConditionalRender) (GLuint id, GLenum mode);
void APIENTRY (*glEndConditionalRender) (void);
void APIENTRY (*glVertexAttribIPointer) (GLuint index, GLint size, GLenum type, GLsizei stride, const void *pointer);
void APIENTRY (*glGetVertexAttribIiv) (GLuint index, GLenum pname, GLint *params);
void APIENTRY (*glGetVertexAttribIuiv) (GLuint index, GLenum pname, GLuint *params);
void APIENTRY (*glVertexAttribI1i) (GLuint index, GLint x);
void APIENTRY (*glVertexAttribI2i) (GLuint index, GLint x, GLint y);
void APIENTRY (*glVertexAttribI3i) (GLuint index, GLint x, GLint y, GLint z);
void APIENTRY (*glVertexAttribI4i) (GLuint index, GLint x, GLint y, GLint z, GLint w);
void APIENTRY (*glVertexAttribI1ui) (GLuint index, GLuint x);
void APIENTRY (*glVertexAttribI2ui) (GLuint index, GLuint x, GLuint y);
void APIENTRY (*glVertexAttribI3ui) (GLuint index, GLuint x, GLuint y, GLuint z);
void APIENTRY (*glVertexAttribI4ui) (GLuint index, GLuint x, GLuint y, GLuint z, GLuint w);
void APIENTRY (*glVertexAttribI1iv) (GLuint index, const GLint *v);
void APIENTRY (*glVertexAttribI2iv) (GLuint index, const GLint *v);
void APIENTRY (*glVertexAttribI3iv) (GLuint index, const GLint *v);
void APIENTRY (*glVertexAttribI4iv) (GLuint index, const GLint *v);
void APIENTRY (*glVertexAttribI1uiv) (GLuint index, const GLuint *v);
void APIENTRY (*glVertexAttribI2uiv) (GLuint index, const GLuint *v);
void APIENTRY (*glVertexAttribI3uiv) (GLuint index, const GLuint *v);
void APIENTRY (*glVertexAttribI4uiv) (GLuint index, const GLuint *v);
void APIENTRY (*glVertexAttribI4bv) (GLuint index, const GLbyte *v);
void APIENTRY (*glVertexAttribI4sv) (GLuint index, const GLshort *v);
void APIENTRY (*glVertexAttribI4ubv) (GLuint index, const GLubyte *v);
void APIENTRY (*glVertexAttribI4usv) (GLuint index, const GLushort *v);
void APIENTRY (*glGetUniformuiv) (GLuint program, GLint location, GLuint *params);
void APIENTRY (*glBindFragDataLocation) (GLuint program, GLuint color, const GLchar *name);
GLint APIENTRY (*glGetFragDataLocation) (GLuint program, const GLchar *name);
void APIENTRY (*glUniform1ui) (GLint location, GLuint v0);
void APIENTRY (*glUniform2ui) (GLint location, GLuint v0, GLuint v1);
void APIENTRY (*glUniform3ui) (GLint location, GLuint v0, GLuint v1, GLuint v2);
void APIENTRY (*glUniform4ui) (GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
void APIENTRY (*glUniform1uiv) (GLint location, GLsizei count, const GLuint *value);
void APIENTRY (*glUniform2uiv) (GLint location, GLsizei count, const GLuint *value);
void APIENTRY (*glUniform3uiv) (GLint location, GLsizei count, const GLuint *value);
void APIENTRY (*glUniform4uiv) (GLint location, GLsizei count, const GLuint *value);
void APIENTRY (*glTexParameterIiv) (GLenum target, GLenum pname, const GLint *params);
void APIENTRY (*glTexParameterIuiv) (GLenum target, GLenum pname, const GLuint *params);
void APIENTRY (*glGetTexParameterIiv) (GLenum target, GLenum pname, GLint *params);
void APIENTRY (*glGetTexParameterIuiv) (GLenum target, GLenum pname, GLuint *params);
void APIENTRY (*glClearBufferiv) (GLenum buffer, GLint drawbuffer, const GLint *value);
void APIENTRY (*glClearBufferuiv) (GLenum buffer, GLint drawbuffer, const GLuint *value);
void APIENTRY (*glClearBufferfv) (GLenum buffer, GLint drawbuffer, const GLfloat *value);
void APIENTRY (*glClearBufferfi) (GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil);
const GLubyte *APIENTRY (*glGetStringi) (GLenum name, GLuint index);
GLboolean APIENTRY (*glIsRenderbuffer) (GLuint renderbuffer);
void APIENTRY (*glBindRenderbuffer) (GLenum target, GLuint renderbuffer);
void APIENTRY (*glDeleteRenderbuffers) (GLsizei n, const GLuint *renderbuffers);
void APIENTRY (*glGenRenderbuffers) (GLsizei n, GLuint *renderbuffers);
void APIENTRY (*glRenderbufferStorage) (GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
void APIENTRY (*glGetRenderbufferParameteriv) (GLenum target, GLenum pname, GLint *params);
GLboolean APIENTRY (*glIsFramebuffer) (GLuint framebuffer);
void APIENTRY (*glBindFramebuffer) (GLenum target, GLuint framebuffer);
void APIENTRY (*glDeleteFramebuffers) (GLsizei n, const GLuint *framebuffers);
void APIENTRY (*glGenFramebuffers) (GLsizei n, GLuint *framebuffers);
GLenum APIENTRY (*glCheckFramebufferStatus) (GLenum target);
void APIENTRY (*glFramebufferTexture1D) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
void APIENTRY (*glFramebufferTexture2D) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
void APIENTRY (*glFramebufferTexture3D) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset);
void APIENTRY (*glFramebufferRenderbuffer) (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
void APIENTRY (*glGetFramebufferAttachmentParameteriv) (GLenum target, GLenum attachment, GLenum pname, GLint *params);
void APIENTRY (*glGenerateMipmap) (GLenum target);
void APIENTRY (*glBlitFramebuffer) (GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
void APIENTRY (*glRenderbufferStorageMultisample) (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
void APIENTRY (*glFramebufferTextureLayer) (GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer);
void *APIENTRY (*glMapBufferRange) (GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access);
void APIENTRY (*glFlushMappedBufferRange) (GLenum target, GLintptr offset, GLsizeiptr length);
void APIENTRY (*glBindVertexArray) (GLuint array);
void APIENTRY (*glDeleteVertexArrays) (GLsizei n, const GLuint *arrays);
void APIENTRY (*glGenVertexArrays) (GLsizei n, GLuint *arrays);
GLboolean APIENTRY (*glIsVertexArray) (GLuint array);
void APIENTRY (*glDrawArraysInstanced) (GLenum mode, GLint first, GLsizei count, GLsizei instancecount);
void APIENTRY (*glDrawElementsInstanced) (GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount);
void APIENTRY (*glTexBuffer) (GLenum target, GLenum internalformat, GLuint buffer);
void APIENTRY (*glPrimitiveRestartIndex) (GLuint index);
void APIENTRY (*glCopyBufferSubData) (GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
void APIENTRY (*glGetUniformIndices) (GLuint program, GLsizei uniformCount, const GLchar *const*uniformNames, GLuint *uniformIndices);
void APIENTRY (*glGetActiveUniformsiv) (GLuint program, GLsizei uniformCount, const GLuint *uniformIndices, GLenum pname, GLint *params);
void APIENTRY (*glGetActiveUniformName) (GLuint program, GLuint uniformIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformName);
GLuint APIENTRY (*glGetUniformBlockIndex) (GLuint program, const GLchar *uniformBlockName);
void APIENTRY (*glGetActiveUniformBlockiv) (GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint *params);
void APIENTRY (*glGetActiveUniformBlockName) (GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformBlockName);
void APIENTRY (*glUniformBlockBinding) (GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);
void APIENTRY (*glDrawElementsBaseVertex) (GLenum mode, GLsizei count, GLenum type, const void *indices, GLint basevertex);
void APIENTRY (*glDrawRangeElementsBaseVertex) (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices, GLint basevertex);
void APIENTRY (*glDrawElementsInstancedBaseVertex) (GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount, GLint basevertex);
void APIENTRY (*glMultiDrawElementsBaseVertex) (GLenum mode, const GLsizei *count, GLenum type, const void *const*indices, GLsizei drawcount, const GLint *basevertex);
void APIENTRY (*glProvokingVertex) (GLenum mode);
GLsync APIENTRY (*glFenceSync) (GLenum condition, GLbitfield flags);
GLboolean APIENTRY (*glIsSync) (GLsync sync);
void APIENTRY (*glDeleteSync) (GLsync sync);
GLenum APIENTRY (*glClientWaitSync) (GLsync sync, GLbitfield flags, GLuint64 timeout);
void APIENTRY (*glWaitSync) (GLsync sync, GLbitfield flags, GLuint64 timeout);
void APIENTRY (*glGetInteger64v) (GLenum pname, GLint64 *data);
void APIENTRY (*glGetSynciv) (GLsync sync, GLenum pname, GLsizei count, GLsizei *length, GLint *values);
void APIENTRY (*glGetInteger64i_v) (GLenum target, GLuint index, GLint64 *data);
void APIENTRY (*glGetBufferParameteri64v) (GLenum target, GLenum pname, GLint64 *params);
void APIENTRY (*glFramebufferTexture) (GLenum target, GLenum attachment, GLuint texture, GLint level);
void APIENTRY (*glTexImage2DMultisample) (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
void APIENTRY (*glTexImage3DMultisample) (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
void APIENTRY (*glGetMultisamplefv) (GLenum pname, GLuint index, GLfloat *val);
void APIENTRY (*glSampleMaski) (GLuint maskNumber, GLbitfield mask);
void APIENTRY (*glBindFragDataLocationIndexed) (GLuint program, GLuint colorNumber, GLuint index, const GLchar *name);
GLint APIENTRY (*glGetFragDataIndex) (GLuint program, const GLchar *name);
void APIENTRY (*glGenSamplers) (GLsizei count, GLuint *samplers);
void APIENTRY (*glDeleteSamplers) (GLsizei count, const GLuint *samplers);
GLboolean APIENTRY (*glIsSampler) (GLuint sampler);
void APIENTRY (*glBindSampler) (GLuint unit, GLuint sampler);
void APIENTRY (*glSamplerParameteri) (GLuint sampler, GLenum pname, GLint param);
void APIENTRY (*glSamplerParameteriv) (GLuint sampler, GLenum pname, const GLint *param);
void APIENTRY (*glSamplerParameterf) (GLuint sampler, GLenum pname, GLfloat param);
void APIENTRY (*glSamplerParameterfv) (GLuint sampler, GLenum pname, const GLfloat *param);
void APIENTRY (*glSamplerParameterIiv) (GLuint sampler, GLenum pname, const GLint *param);
void APIENTRY (*glSamplerParameterIuiv) (GLuint sampler, GLenum pname, const GLuint *param);
void APIENTRY (*glGetSamplerParameteriv) (GLuint sampler, GLenum pname, GLint *params);
void APIENTRY (*glGetSamplerParameterIiv) (GLuint sampler, GLenum pname, GLint *params);
void APIENTRY (*glGetSamplerParameterfv) (GLuint sampler, GLenum pname, GLfloat *params);
void APIENTRY (*glGetSamplerParameterIuiv) (GLuint sampler, GLenum pname, GLuint *params);
void APIENTRY (*glQueryCounter) (GLuint id, GLenum target);
void APIENTRY (*glGetQueryObjecti64v) (GLuint id, GLenum pname, GLint64 *params);
void APIENTRY (*glGetQueryObjectui64v) (GLuint id, GLenum pname, GLuint64 *params);
void APIENTRY (*glVertexAttribDivisor) (GLuint index, GLuint divisor);
void APIENTRY (*glVertexAttribP1ui) (GLuint index, GLenum type, GLboolean normalized, GLuint value);
void APIENTRY (*glVertexAttribP1uiv) (GLuint index, GLenum type, GLboolean normalized, const GLuint *value);
void APIENTRY (*glVertexAttribP2ui) (GLuint index, GLenum type, GLboolean normalized, GLuint value);
void APIENTRY (*glVertexAttribP2uiv) (GLuint index, GLenum type, GLboolean normalized, const GLuint *value);
void APIENTRY (*glVertexAttribP3ui) (GLuint index, GLenum type, GLboolean normalized, GLuint value);
void APIENTRY (*glVertexAttribP3uiv) (GLuint index, GLenum type, GLboolean normalized, const GLuint *value);
void APIENTRY (*glVertexAttribP4ui) (GLuint index, GLenum type, GLboolean normalized, GLuint value);
void APIENTRY (*glVertexAttribP4uiv) (GLuint index, GLenum type, GLboolean normalized, const GLuint *value);
void APIENTRY (*glMinSampleShading) (GLfloat value);
void APIENTRY (*glBlendEquationi) (GLuint buf, GLenum mode);
void APIENTRY (*glBlendEquationSeparatei) (GLuint buf, GLenum modeRGB, GLenum modeAlpha);
void APIENTRY (*glBlendFunci) (GLuint buf, GLenum src, GLenum dst);
void APIENTRY (*glBlendFuncSeparatei) (GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
void APIENTRY (*glDrawArraysIndirect) (GLenum mode, const void *indirect);
void APIENTRY (*glDrawElementsIndirect) (GLenum mode, GLenum type, const void *indirect);
void APIENTRY (*glUniform1d) (GLint location, GLdouble x);
void APIENTRY (*glUniform2d) (GLint location, GLdouble x, GLdouble y);
void APIENTRY (*glUniform3d) (GLint location, GLdouble x, GLdouble y, GLdouble z);
void APIENTRY (*glUniform4d) (GLint location, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
void APIENTRY (*glUniform1dv) (GLint location, GLsizei count, const GLdouble *value);
void APIENTRY (*glUniform2dv) (GLint location, GLsizei count, const GLdouble *value);
void APIENTRY (*glUniform3dv) (GLint location, GLsizei count, const GLdouble *value);
void APIENTRY (*glUniform4dv) (GLint location, GLsizei count, const GLdouble *value);
void APIENTRY (*glUniformMatrix2dv) (GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
void APIENTRY (*glUniformMatrix3dv) (GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
void APIENTRY (*glUniformMatrix4dv) (GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
void APIENTRY (*glUniformMatrix2x3dv) (GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
void APIENTRY (*glUniformMatrix2x4dv) (GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
void APIENTRY (*glUniformMatrix3x2dv) (GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
void APIENTRY (*glUniformMatrix3x4dv) (GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
void APIENTRY (*glUniformMatrix4x2dv) (GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
void APIENTRY (*glUniformMatrix4x3dv) (GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
void APIENTRY (*glGetUniformdv) (GLuint program, GLint location, GLdouble *params);
GLint APIENTRY (*glGetSubroutineUniformLocation) (GLuint program, GLenum shadertype, const GLchar *name);
GLuint APIENTRY (*glGetSubroutineIndex) (GLuint program, GLenum shadertype, const GLchar *name);
void APIENTRY (*glGetActiveSubroutineUniformiv) (GLuint program, GLenum shadertype, GLuint index, GLenum pname, GLint *values);
void APIENTRY (*glGetActiveSubroutineUniformName) (GLuint program, GLenum shadertype, GLuint index, GLsizei bufSize, GLsizei *length, GLchar *name);
void APIENTRY (*glGetActiveSubroutineName) (GLuint program, GLenum shadertype, GLuint index, GLsizei bufSize, GLsizei *length, GLchar *name);
void APIENTRY (*glUniformSubroutinesuiv) (GLenum shadertype, GLsizei count, const GLuint *indices);
void APIENTRY (*glGetUniformSubroutineuiv) (GLenum shadertype, GLint location, GLuint *params);
void APIENTRY (*glGetProgramStageiv) (GLuint program, GLenum shadertype, GLenum pname, GLint *values);
void APIENTRY (*glPatchParameteri) (GLenum pname, GLint value);
void APIENTRY (*glPatchParameterfv) (GLenum pname, const GLfloat *values);
void APIENTRY (*glBindTransformFeedback) (GLenum target, GLuint id);
void APIENTRY (*glDeleteTransformFeedbacks) (GLsizei n, const GLuint *ids);
void APIENTRY (*glGenTransformFeedbacks) (GLsizei n, GLuint *ids);
GLboolean APIENTRY (*glIsTransformFeedback) (GLuint id);
void APIENTRY (*glPauseTransformFeedback) (void);
void APIENTRY (*glResumeTransformFeedback) (void);
void APIENTRY (*glDrawTransformFeedback) (GLenum mode, GLuint id);
void APIENTRY (*glDrawTransformFeedbackStream) (GLenum mode, GLuint id, GLuint stream);
void APIENTRY (*glBeginQueryIndexed) (GLenum target, GLuint index, GLuint id);
void APIENTRY (*glEndQueryIndexed) (GLenum target, GLuint index);
void APIENTRY (*glGetQueryIndexediv) (GLenum target, GLuint index, GLenum pname, GLint *params);
void APIENTRY (*glDebugMessageCallbackARB) (GLDEBUGPROCARB callback, const void *userParam);
};

static void gl_api_load(gl_api *gl, void *load(const char *)) {
#define load_new(k) gl->k = load(#k); assert(gl->k);
#if OS_WINDOWS
#define load_old(k) gl->k = k; assert(gl->k);
#else
#define load_old(k) load_new(k)
#endif
    load_old(glCullFace);
    load_old(glFrontFace);
    load_old(glHint);
    load_old(glLineWidth);
    load_old(glPointSize);
    load_old(glPolygonMode);
    load_old(glScissor);
    load_old(glTexParameterf);
    load_old(glTexParameterfv);
    load_old(glTexParameteri);
    load_old(glTexParameteriv);
    load_old(glTexImage1D);
    load_old(glTexImage2D);
    load_old(glDrawBuffer);
    load_old(glClear);
    load_old(glClearColor);
    load_old(glClearStencil);
    load_old(glClearDepth);
    load_old(glStencilMask);
    load_old(glColorMask);
    load_old(glDepthMask);
    load_old(glDisable);
    load_old(glEnable);
    load_old(glFinish);
    load_old(glFlush);
    load_old(glBlendFunc);
    load_old(glLogicOp);
    load_old(glStencilFunc);
    load_old(glStencilOp);
    load_old(glDepthFunc);
    load_old(glPixelStoref);
    load_old(glPixelStorei);
    load_old(glReadBuffer);
    load_old(glReadPixels);
    load_old(glGetBooleanv);
    load_old(glGetDoublev);
    load_old(glGetError);
    load_old(glGetFloatv);
    load_old(glGetIntegerv);
    load_old(glGetTexImage);
    load_old(glGetTexParameterfv);
    load_old(glGetTexParameteriv);
    load_old(glGetTexLevelParameterfv);
    load_old(glGetTexLevelParameteriv);
    load_old(glIsEnabled);
    load_old(glDepthRange);
    load_old(glViewport);
    load_old(glDrawArrays);
    load_old(glDrawElements);
    load_old(glGetPointerv);
    load_old(glPolygonOffset);
    load_old(glCopyTexImage1D);
    load_old(glCopyTexImage2D);
    load_old(glCopyTexSubImage1D);
    load_old(glCopyTexSubImage2D);
    load_old(glTexSubImage1D);
    load_old(glTexSubImage2D);
    load_old(glBindTexture);
    load_old(glDeleteTextures);
    load_old(glGenTextures);
    load_old(glIsTexture);
    load_new(glDrawRangeElements);
    load_new(glTexImage3D);
    load_new(glTexSubImage3D);
    load_new(glCopyTexSubImage3D);
    load_new(glActiveTexture);
    load_new(glSampleCoverage);
    load_new(glCompressedTexImage3D);
    load_new(glCompressedTexImage2D);
    load_new(glCompressedTexImage1D);
    load_new(glCompressedTexSubImage3D);
    load_new(glCompressedTexSubImage2D);
    load_new(glCompressedTexSubImage1D);
    load_new(glGetCompressedTexImage);
    load_new(glBlendFuncSeparate);
    load_new(glMultiDrawArrays);
    load_new(glMultiDrawElements);
    load_new(glPointParameterf);
    load_new(glPointParameterfv);
    load_new(glPointParameteri);
    load_new(glPointParameteriv);
    load_new(glBlendColor);
    load_new(glBlendEquation);
    load_new(glGenQueries);
    load_new(glDeleteQueries);
    load_new(glIsQuery);
    load_new(glBeginQuery);
    load_new(glEndQuery);
    load_new(glGetQueryiv);
    load_new(glGetQueryObjectiv);
    load_new(glGetQueryObjectuiv);
    load_new(glBindBuffer);
    load_new(glDeleteBuffers);
    load_new(glGenBuffers);
    load_new(glIsBuffer);
    load_new(glBufferData);
    load_new(glBufferSubData);
    load_new(glGetBufferSubData);
    load_new(glUnmapBuffer);
    load_new(glGetBufferParameteriv);
    load_new(glGetBufferPointerv);
    load_new(glBlendEquationSeparate);
    load_new(glDrawBuffers);
    load_new(glStencilOpSeparate);
    load_new(glStencilFuncSeparate);
    load_new(glStencilMaskSeparate);
    load_new(glAttachShader);
    load_new(glBindAttribLocation);
    load_new(glCompileShader);
    load_new(glCreateProgram);
    load_new(glCreateShader);
    load_new(glDeleteProgram);
    load_new(glDeleteShader);
    load_new(glDetachShader);
    load_new(glDisableVertexAttribArray);
    load_new(glEnableVertexAttribArray);
    load_new(glGetActiveAttrib);
    load_new(glGetActiveUniform);
    load_new(glGetAttachedShaders);
    load_new(glGetAttribLocation);
    load_new(glGetProgramiv);
    load_new(glGetProgramInfoLog);
    load_new(glGetShaderiv);
    load_new(glGetShaderInfoLog);
    load_new(glGetShaderSource);
    load_new(glGetUniformLocation);
    load_new(glGetUniformfv);
    load_new(glGetUniformiv);
    load_new(glGetVertexAttribdv);
    load_new(glGetVertexAttribfv);
    load_new(glGetVertexAttribiv);
    load_new(glGetVertexAttribPointerv);
    load_new(glIsProgram);
    load_new(glIsShader);
    load_new(glLinkProgram);
    load_new(glShaderSource);
    load_new(glUseProgram);
    load_new(glUniform1f);
    load_new(glUniform2f);
    load_new(glUniform3f);
    load_new(glUniform4f);
    load_new(glUniform1i);
    load_new(glUniform2i);
    load_new(glUniform3i);
    load_new(glUniform4i);
    load_new(glUniform1fv);
    load_new(glUniform2fv);
    load_new(glUniform3fv);
    load_new(glUniform4fv);
    load_new(glUniform1iv);
    load_new(glUniform2iv);
    load_new(glUniform3iv);
    load_new(glUniform4iv);
    load_new(glUniformMatrix2fv);
    load_new(glUniformMatrix3fv);
    load_new(glUniformMatrix4fv);
    load_new(glValidateProgram);
    load_new(glVertexAttrib1d);
    load_new(glVertexAttrib1dv);
    load_new(glVertexAttrib1f);
    load_new(glVertexAttrib1fv);
    load_new(glVertexAttrib1s);
    load_new(glVertexAttrib1sv);
    load_new(glVertexAttrib2d);
    load_new(glVertexAttrib2dv);
    load_new(glVertexAttrib2f);
    load_new(glVertexAttrib2fv);
    load_new(glVertexAttrib2s);
    load_new(glVertexAttrib2sv);
    load_new(glVertexAttrib3d);
    load_new(glVertexAttrib3dv);
    load_new(glVertexAttrib3f);
    load_new(glVertexAttrib3fv);
    load_new(glVertexAttrib3s);
    load_new(glVertexAttrib3sv);
    load_new(glVertexAttrib4Nbv);
    load_new(glVertexAttrib4Niv);
    load_new(glVertexAttrib4Nsv);
    load_new(glVertexAttrib4Nub);
    load_new(glVertexAttrib4Nubv);
    load_new(glVertexAttrib4Nuiv);
    load_new(glVertexAttrib4Nusv);
    load_new(glVertexAttrib4bv);
    load_new(glVertexAttrib4d);
    load_new(glVertexAttrib4dv);
    load_new(glVertexAttrib4f);
    load_new(glVertexAttrib4fv);
    load_new(glVertexAttrib4iv);
    load_new(glVertexAttrib4s);
    load_new(glVertexAttrib4sv);
    load_new(glVertexAttrib4ubv);
    load_new(glVertexAttrib4uiv);
    load_new(glVertexAttrib4usv);
    load_new(glVertexAttribPointer);
    load_new(glUniformMatrix2x3fv);
    load_new(glUniformMatrix3x2fv);
    load_new(glUniformMatrix2x4fv);
    load_new(glUniformMatrix4x2fv);
    load_new(glUniformMatrix3x4fv);
    load_new(glUniformMatrix4x3fv);
    load_new(glColorMaski);
    load_new(glGetBooleani_v);
    load_new(glGetIntegeri_v);
    load_new(glEnablei);
    load_new(glDisablei);
    load_new(glIsEnabledi);
    load_new(glBeginTransformFeedback);
    load_new(glEndTransformFeedback);
    load_new(glBindBufferRange);
    load_new(glBindBufferBase);
    load_new(glTransformFeedbackVaryings);
    load_new(glGetTransformFeedbackVarying);
    load_new(glClampColor);
    load_new(glBeginConditionalRender);
    load_new(glEndConditionalRender);
    load_new(glVertexAttribIPointer);
    load_new(glGetVertexAttribIiv);
    load_new(glGetVertexAttribIuiv);
    load_new(glVertexAttribI1i);
    load_new(glVertexAttribI2i);
    load_new(glVertexAttribI3i);
    load_new(glVertexAttribI4i);
    load_new(glVertexAttribI1ui);
    load_new(glVertexAttribI2ui);
    load_new(glVertexAttribI3ui);
    load_new(glVertexAttribI4ui);
    load_new(glVertexAttribI1iv);
    load_new(glVertexAttribI2iv);
    load_new(glVertexAttribI3iv);
    load_new(glVertexAttribI4iv);
    load_new(glVertexAttribI1uiv);
    load_new(glVertexAttribI2uiv);
    load_new(glVertexAttribI3uiv);
    load_new(glVertexAttribI4uiv);
    load_new(glVertexAttribI4bv);
    load_new(glVertexAttribI4sv);
    load_new(glVertexAttribI4ubv);
    load_new(glVertexAttribI4usv);
    load_new(glGetUniformuiv);
    load_new(glBindFragDataLocation);
    load_new(glGetFragDataLocation);
    load_new(glUniform1ui);
    load_new(glUniform2ui);
    load_new(glUniform3ui);
    load_new(glUniform4ui);
    load_new(glUniform1uiv);
    load_new(glUniform2uiv);
    load_new(glUniform3uiv);
    load_new(glUniform4uiv);
    load_new(glTexParameterIiv);
    load_new(glTexParameterIuiv);
    load_new(glGetTexParameterIiv);
    load_new(glGetTexParameterIuiv);
    load_new(glClearBufferiv);
    load_new(glClearBufferuiv);
    load_new(glClearBufferfv);
    load_new(glClearBufferfi);
    load_new(glIsRenderbuffer);
    load_new(glBindRenderbuffer);
    load_new(glDeleteRenderbuffers);
    load_new(glGenRenderbuffers);
    load_new(glRenderbufferStorage);
    load_new(glGetRenderbufferParameteriv);
    load_new(glIsFramebuffer);
    load_new(glBindFramebuffer);
    load_new(glDeleteFramebuffers);
    load_new(glGenFramebuffers);
    load_new(glCheckFramebufferStatus);
    load_new(glFramebufferTexture1D);
    load_new(glFramebufferTexture2D);
    load_new(glFramebufferTexture3D);
    load_new(glFramebufferRenderbuffer);
    load_new(glGetFramebufferAttachmentParameteriv);
    load_new(glGenerateMipmap);
    load_new(glBlitFramebuffer);
    load_new(glRenderbufferStorageMultisample);
    load_new(glFramebufferTextureLayer);
    load_new(glFlushMappedBufferRange);
    load_new(glBindVertexArray);
    load_new(glDeleteVertexArrays);
    load_new(glGenVertexArrays);
    load_new(glIsVertexArray);
    load_new(glDrawArraysInstanced);
    load_new(glDrawElementsInstanced);
    load_new(glTexBuffer);
    load_new(glPrimitiveRestartIndex);
    load_new(glCopyBufferSubData);
    load_new(glGetUniformIndices);
    load_new(glGetActiveUniformsiv);
    load_new(glGetActiveUniformName);
    load_new(glGetUniformBlockIndex);
    load_new(glGetActiveUniformBlockiv);
    load_new(glGetActiveUniformBlockName);
    load_new(glUniformBlockBinding);
    load_new(glDrawElementsBaseVertex);
    load_new(glDrawRangeElementsBaseVertex);
    load_new(glDrawElementsInstancedBaseVertex);
    load_new(glMultiDrawElementsBaseVertex);
    load_new(glProvokingVertex);
    load_new(glFenceSync);
    load_new(glIsSync);
    load_new(glDeleteSync);
    load_new(glClientWaitSync);
    load_new(glWaitSync);
    load_new(glGetInteger64v);
    load_new(glGetSynciv);
    load_new(glGetInteger64i_v);
    load_new(glGetBufferParameteri64v);
    load_new(glFramebufferTexture);
    load_new(glTexImage2DMultisample);
    load_new(glTexImage3DMultisample);
    load_new(glGetMultisamplefv);
    load_new(glSampleMaski);
    load_new(glBindFragDataLocationIndexed);
    load_new(glGetFragDataIndex);
    load_new(glGenSamplers);
    load_new(glDeleteSamplers);
    load_new(glIsSampler);
    load_new(glBindSampler);
    load_new(glSamplerParameteri);
    load_new(glSamplerParameteriv);
    load_new(glSamplerParameterf);
    load_new(glSamplerParameterfv);
    load_new(glSamplerParameterIiv);
    load_new(glSamplerParameterIuiv);
    load_new(glGetSamplerParameteriv);
    load_new(glGetSamplerParameterIiv);
    load_new(glGetSamplerParameterfv);
    load_new(glGetSamplerParameterIuiv);
    load_new(glQueryCounter);
    load_new(glGetQueryObjecti64v);
    load_new(glGetQueryObjectui64v);
    load_new(glVertexAttribDivisor);
    load_new(glVertexAttribP1ui);
    load_new(glVertexAttribP1uiv);
    load_new(glVertexAttribP2ui);
    load_new(glVertexAttribP2uiv);
    load_new(glVertexAttribP3ui);
    load_new(glVertexAttribP3uiv);
    load_new(glVertexAttribP4ui);
    load_new(glVertexAttribP4uiv);
    load_new(glMinSampleShading);
    load_new(glBlendEquationi);
    load_new(glBlendEquationSeparatei);
    load_new(glBlendFunci);
    load_new(glBlendFuncSeparatei);
    load_new(glDrawArraysIndirect);
    load_new(glDrawElementsIndirect);
    load_new(glUniform1d);
    load_new(glUniform2d);
    load_new(glUniform3d);
    load_new(glUniform4d);
    load_new(glUniform1dv);
    load_new(glUniform2dv);
    load_new(glUniform3dv);
    load_new(glUniform4dv);
    load_new(glUniformMatrix2dv);
    load_new(glUniformMatrix3dv);
    load_new(glUniformMatrix4dv);
    load_new(glUniformMatrix2x3dv);
    load_new(glUniformMatrix2x4dv);
    load_new(glUniformMatrix3x2dv);
    load_new(glUniformMatrix3x4dv);
    load_new(glUniformMatrix4x2dv);
    load_new(glUniformMatrix4x3dv);
    load_new(glGetUniformdv);
    load_new(glDebugMessageCallbackARB);
}
