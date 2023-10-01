#include "game/math.h"
#include "global/const.h"
#include "util.h"

static const int16_t m_SinTable[0x402] = {
    // clang-format off
    0x0000, 0x0019, 0x0032, 0x004B, 0x0065, 0x007E, 0x0097, 0x00B0,
    0x00C9, 0x00E2, 0x00FB, 0x0114, 0x012E, 0x0147, 0x0160, 0x0179,
    0x0192, 0x01AB, 0x01C4, 0x01DD, 0x01F7, 0x0210, 0x0229, 0x0242,
    0x025B, 0x0274, 0x028D, 0x02A6, 0x02C0, 0x02D9, 0x02F2, 0x030B,
    0x0324, 0x033D, 0x0356, 0x036F, 0x0388, 0x03A1, 0x03BB, 0x03D4,
    0x03ED, 0x0406, 0x041F, 0x0438, 0x0451, 0x046A, 0x0483, 0x049C,
    0x04B5, 0x04CE, 0x04E7, 0x0500, 0x051A, 0x0533, 0x054C, 0x0565,
    0x057E, 0x0597, 0x05B0, 0x05C9, 0x05E2, 0x05FB, 0x0614, 0x062D,
    0x0646, 0x065F, 0x0678, 0x0691, 0x06AA, 0x06C3, 0x06DC, 0x06F5,
    0x070E, 0x0727, 0x0740, 0x0759, 0x0772, 0x078B, 0x07A4, 0x07BD,
    0x07D6, 0x07EF, 0x0807, 0x0820, 0x0839, 0x0852, 0x086B, 0x0884,
    0x089D, 0x08B6, 0x08CF, 0x08E8, 0x0901, 0x0919, 0x0932, 0x094B,
    0x0964, 0x097D, 0x0996, 0x09AF, 0x09C7, 0x09E0, 0x09F9, 0x0A12,
    0x0A2B, 0x0A44, 0x0A5C, 0x0A75, 0x0A8E, 0x0AA7, 0x0AC0, 0x0AD8,
    0x0AF1, 0x0B0A, 0x0B23, 0x0B3B, 0x0B54, 0x0B6D, 0x0B85, 0x0B9E,
    0x0BB7, 0x0BD0, 0x0BE8, 0x0C01, 0x0C1A, 0x0C32, 0x0C4B, 0x0C64,
    0x0C7C, 0x0C95, 0x0CAE, 0x0CC6, 0x0CDF, 0x0CF8, 0x0D10, 0x0D29,
    0x0D41, 0x0D5A, 0x0D72, 0x0D8B, 0x0DA4, 0x0DBC, 0x0DD5, 0x0DED,
    0x0E06, 0x0E1E, 0x0E37, 0x0E4F, 0x0E68, 0x0E80, 0x0E99, 0x0EB1,
    0x0ECA, 0x0EE2, 0x0EFB, 0x0F13, 0x0F2B, 0x0F44, 0x0F5C, 0x0F75,
    0x0F8D, 0x0FA5, 0x0FBE, 0x0FD6, 0x0FEE, 0x1007, 0x101F, 0x1037,
    0x1050, 0x1068, 0x1080, 0x1099, 0x10B1, 0x10C9, 0x10E1, 0x10FA,
    0x1112, 0x112A, 0x1142, 0x115A, 0x1173, 0x118B, 0x11A3, 0x11BB,
    0x11D3, 0x11EB, 0x1204, 0x121C, 0x1234, 0x124C, 0x1264, 0x127C,
    0x1294, 0x12AC, 0x12C4, 0x12DC, 0x12F4, 0x130C, 0x1324, 0x133C,
    0x1354, 0x136C, 0x1384, 0x139C, 0x13B4, 0x13CC, 0x13E4, 0x13FB,
    0x1413, 0x142B, 0x1443, 0x145B, 0x1473, 0x148B, 0x14A2, 0x14BA,
    0x14D2, 0x14EA, 0x1501, 0x1519, 0x1531, 0x1549, 0x1560, 0x1578,
    0x1590, 0x15A7, 0x15BF, 0x15D7, 0x15EE, 0x1606, 0x161D, 0x1635,
    0x164C, 0x1664, 0x167C, 0x1693, 0x16AB, 0x16C2, 0x16DA, 0x16F1,
    0x1709, 0x1720, 0x1737, 0x174F, 0x1766, 0x177E, 0x1795, 0x17AC,
    0x17C4, 0x17DB, 0x17F2, 0x180A, 0x1821, 0x1838, 0x184F, 0x1867,
    0x187E, 0x1895, 0x18AC, 0x18C3, 0x18DB, 0x18F2, 0x1909, 0x1920,
    0x1937, 0x194E, 0x1965, 0x197C, 0x1993, 0x19AA, 0x19C1, 0x19D8,
    0x19EF, 0x1A06, 0x1A1D, 0x1A34, 0x1A4B, 0x1A62, 0x1A79, 0x1A90,
    0x1AA7, 0x1ABE, 0x1AD4, 0x1AEB, 0x1B02, 0x1B19, 0x1B30, 0x1B46,
    0x1B5D, 0x1B74, 0x1B8A, 0x1BA1, 0x1BB8, 0x1BCE, 0x1BE5, 0x1BFC,
    0x1C12, 0x1C29, 0x1C3F, 0x1C56, 0x1C6C, 0x1C83, 0x1C99, 0x1CB0,
    0x1CC6, 0x1CDD, 0x1CF3, 0x1D0A, 0x1D20, 0x1D36, 0x1D4D, 0x1D63,
    0x1D79, 0x1D90, 0x1DA6, 0x1DBC, 0x1DD3, 0x1DE9, 0x1DFF, 0x1E15,
    0x1E2B, 0x1E42, 0x1E58, 0x1E6E, 0x1E84, 0x1E9A, 0x1EB0, 0x1EC6,
    0x1EDC, 0x1EF2, 0x1F08, 0x1F1E, 0x1F34, 0x1F4A, 0x1F60, 0x1F76,
    0x1F8C, 0x1FA2, 0x1FB7, 0x1FCD, 0x1FE3, 0x1FF9, 0x200F, 0x2024,
    0x203A, 0x2050, 0x2065, 0x207B, 0x2091, 0x20A6, 0x20BC, 0x20D1,
    0x20E7, 0x20FD, 0x2112, 0x2128, 0x213D, 0x2153, 0x2168, 0x217D,
    0x2193, 0x21A8, 0x21BE, 0x21D3, 0x21E8, 0x21FE, 0x2213, 0x2228,
    0x223D, 0x2253, 0x2268, 0x227D, 0x2292, 0x22A7, 0x22BC, 0x22D2,
    0x22E7, 0x22FC, 0x2311, 0x2326, 0x233B, 0x2350, 0x2365, 0x237A,
    0x238E, 0x23A3, 0x23B8, 0x23CD, 0x23E2, 0x23F7, 0x240B, 0x2420,
    0x2435, 0x244A, 0x245E, 0x2473, 0x2488, 0x249C, 0x24B1, 0x24C5,
    0x24DA, 0x24EF, 0x2503, 0x2518, 0x252C, 0x2541, 0x2555, 0x2569,
    0x257E, 0x2592, 0x25A6, 0x25BB, 0x25CF, 0x25E3, 0x25F8, 0x260C,
    0x2620, 0x2634, 0x2648, 0x265C, 0x2671, 0x2685, 0x2699, 0x26AD,
    0x26C1, 0x26D5, 0x26E9, 0x26FD, 0x2711, 0x2724, 0x2738, 0x274C,
    0x2760, 0x2774, 0x2788, 0x279B, 0x27AF, 0x27C3, 0x27D6, 0x27EA,
    0x27FE, 0x2811, 0x2825, 0x2838, 0x284C, 0x2860, 0x2873, 0x2886,
    0x289A, 0x28AD, 0x28C1, 0x28D4, 0x28E7, 0x28FB, 0x290E, 0x2921,
    0x2935, 0x2948, 0x295B, 0x296E, 0x2981, 0x2994, 0x29A7, 0x29BB,
    0x29CE, 0x29E1, 0x29F4, 0x2A07, 0x2A1A, 0x2A2C, 0x2A3F, 0x2A52,
    0x2A65, 0x2A78, 0x2A8B, 0x2A9D, 0x2AB0, 0x2AC3, 0x2AD6, 0x2AE8,
    0x2AFB, 0x2B0D, 0x2B20, 0x2B33, 0x2B45, 0x2B58, 0x2B6A, 0x2B7D,
    0x2B8F, 0x2BA1, 0x2BB4, 0x2BC6, 0x2BD8, 0x2BEB, 0x2BFD, 0x2C0F,
    0x2C21, 0x2C34, 0x2C46, 0x2C58, 0x2C6A, 0x2C7C, 0x2C8E, 0x2CA0,
    0x2CB2, 0x2CC4, 0x2CD6, 0x2CE8, 0x2CFA, 0x2D0C, 0x2D1E, 0x2D2F,
    0x2D41, 0x2D53, 0x2D65, 0x2D76, 0x2D88, 0x2D9A, 0x2DAB, 0x2DBD,
    0x2DCF, 0x2DE0, 0x2DF2, 0x2E03, 0x2E15, 0x2E26, 0x2E37, 0x2E49,
    0x2E5A, 0x2E6B, 0x2E7D, 0x2E8E, 0x2E9F, 0x2EB0, 0x2EC2, 0x2ED3,
    0x2EE4, 0x2EF5, 0x2F06, 0x2F17, 0x2F28, 0x2F39, 0x2F4A, 0x2F5B,
    0x2F6C, 0x2F7D, 0x2F8D, 0x2F9E, 0x2FAF, 0x2FC0, 0x2FD0, 0x2FE1,
    0x2FF2, 0x3002, 0x3013, 0x3024, 0x3034, 0x3045, 0x3055, 0x3066,
    0x3076, 0x3087, 0x3097, 0x30A7, 0x30B8, 0x30C8, 0x30D8, 0x30E8,
    0x30F9, 0x3109, 0x3119, 0x3129, 0x3139, 0x3149, 0x3159, 0x3169,
    0x3179, 0x3189, 0x3199, 0x31A9, 0x31B9, 0x31C8, 0x31D8, 0x31E8,
    0x31F8, 0x3207, 0x3217, 0x3227, 0x3236, 0x3246, 0x3255, 0x3265,
    0x3274, 0x3284, 0x3293, 0x32A3, 0x32B2, 0x32C1, 0x32D0, 0x32E0,
    0x32EF, 0x32FE, 0x330D, 0x331D, 0x332C, 0x333B, 0x334A, 0x3359,
    0x3368, 0x3377, 0x3386, 0x3395, 0x33A3, 0x33B2, 0x33C1, 0x33D0,
    0x33DF, 0x33ED, 0x33FC, 0x340B, 0x3419, 0x3428, 0x3436, 0x3445,
    0x3453, 0x3462, 0x3470, 0x347F, 0x348D, 0x349B, 0x34AA, 0x34B8,
    0x34C6, 0x34D4, 0x34E2, 0x34F1, 0x34FF, 0x350D, 0x351B, 0x3529,
    0x3537, 0x3545, 0x3553, 0x3561, 0x356E, 0x357C, 0x358A, 0x3598,
    0x35A5, 0x35B3, 0x35C1, 0x35CE, 0x35DC, 0x35EA, 0x35F7, 0x3605,
    0x3612, 0x3620, 0x362D, 0x363A, 0x3648, 0x3655, 0x3662, 0x366F,
    0x367D, 0x368A, 0x3697, 0x36A4, 0x36B1, 0x36BE, 0x36CB, 0x36D8,
    0x36E5, 0x36F2, 0x36FF, 0x370C, 0x3718, 0x3725, 0x3732, 0x373F,
    0x374B, 0x3758, 0x3765, 0x3771, 0x377E, 0x378A, 0x3797, 0x37A3,
    0x37B0, 0x37BC, 0x37C8, 0x37D5, 0x37E1, 0x37ED, 0x37F9, 0x3805,
    0x3812, 0x381E, 0x382A, 0x3836, 0x3842, 0x384E, 0x385A, 0x3866,
    0x3871, 0x387D, 0x3889, 0x3895, 0x38A1, 0x38AC, 0x38B8, 0x38C3,
    0x38CF, 0x38DB, 0x38E6, 0x38F2, 0x38FD, 0x3909, 0x3914, 0x391F,
    0x392B, 0x3936, 0x3941, 0x394C, 0x3958, 0x3963, 0x396E, 0x3979,
    0x3984, 0x398F, 0x399A, 0x39A5, 0x39B0, 0x39BB, 0x39C5, 0x39D0,
    0x39DB, 0x39E6, 0x39F0, 0x39FB, 0x3A06, 0x3A10, 0x3A1B, 0x3A25,
    0x3A30, 0x3A3A, 0x3A45, 0x3A4F, 0x3A59, 0x3A64, 0x3A6E, 0x3A78,
    0x3A82, 0x3A8D, 0x3A97, 0x3AA1, 0x3AAB, 0x3AB5, 0x3ABF, 0x3AC9,
    0x3AD3, 0x3ADD, 0x3AE6, 0x3AF0, 0x3AFA, 0x3B04, 0x3B0E, 0x3B17,
    0x3B21, 0x3B2A, 0x3B34, 0x3B3E, 0x3B47, 0x3B50, 0x3B5A, 0x3B63,
    0x3B6D, 0x3B76, 0x3B7F, 0x3B88, 0x3B92, 0x3B9B, 0x3BA4, 0x3BAD,
    0x3BB6, 0x3BBF, 0x3BC8, 0x3BD1, 0x3BDA, 0x3BE3, 0x3BEC, 0x3BF5,
    0x3BFD, 0x3C06, 0x3C0F, 0x3C17, 0x3C20, 0x3C29, 0x3C31, 0x3C3A,
    0x3C42, 0x3C4B, 0x3C53, 0x3C5B, 0x3C64, 0x3C6C, 0x3C74, 0x3C7D,
    0x3C85, 0x3C8D, 0x3C95, 0x3C9D, 0x3CA5, 0x3CAD, 0x3CB5, 0x3CBD,
    0x3CC5, 0x3CCD, 0x3CD5, 0x3CDD, 0x3CE4, 0x3CEC, 0x3CF4, 0x3CFB,
    0x3D03, 0x3D0B, 0x3D12, 0x3D1A, 0x3D21, 0x3D28, 0x3D30, 0x3D37,
    0x3D3F, 0x3D46, 0x3D4D, 0x3D54, 0x3D5B, 0x3D63, 0x3D6A, 0x3D71,
    0x3D78, 0x3D7F, 0x3D86, 0x3D8D, 0x3D93, 0x3D9A, 0x3DA1, 0x3DA8,
    0x3DAF, 0x3DB5, 0x3DBC, 0x3DC2, 0x3DC9, 0x3DD0, 0x3DD6, 0x3DDD,
    0x3DE3, 0x3DE9, 0x3DF0, 0x3DF6, 0x3DFC, 0x3E03, 0x3E09, 0x3E0F,
    0x3E15, 0x3E1B, 0x3E21, 0x3E27, 0x3E2D, 0x3E33, 0x3E39, 0x3E3F,
    0x3E45, 0x3E4A, 0x3E50, 0x3E56, 0x3E5C, 0x3E61, 0x3E67, 0x3E6C,
    0x3E72, 0x3E77, 0x3E7D, 0x3E82, 0x3E88, 0x3E8D, 0x3E92, 0x3E98,
    0x3E9D, 0x3EA2, 0x3EA7, 0x3EAC, 0x3EB1, 0x3EB6, 0x3EBB, 0x3EC0,
    0x3EC5, 0x3ECA, 0x3ECF, 0x3ED4, 0x3ED8, 0x3EDD, 0x3EE2, 0x3EE7,
    0x3EEB, 0x3EF0, 0x3EF4, 0x3EF9, 0x3EFD, 0x3F02, 0x3F06, 0x3F0A,
    0x3F0F, 0x3F13, 0x3F17, 0x3F1C, 0x3F20, 0x3F24, 0x3F28, 0x3F2C,
    0x3F30, 0x3F34, 0x3F38, 0x3F3C, 0x3F40, 0x3F43, 0x3F47, 0x3F4B,
    0x3F4F, 0x3F52, 0x3F56, 0x3F5A, 0x3F5D, 0x3F61, 0x3F64, 0x3F68,
    0x3F6B, 0x3F6E, 0x3F72, 0x3F75, 0x3F78, 0x3F7B, 0x3F7F, 0x3F82,
    0x3F85, 0x3F88, 0x3F8B, 0x3F8E, 0x3F91, 0x3F94, 0x3F97, 0x3F99,
    0x3F9C, 0x3F9F, 0x3FA2, 0x3FA4, 0x3FA7, 0x3FAA, 0x3FAC, 0x3FAF,
    0x3FB1, 0x3FB4, 0x3FB6, 0x3FB8, 0x3FBB, 0x3FBD, 0x3FBF, 0x3FC1,
    0x3FC4, 0x3FC6, 0x3FC8, 0x3FCA, 0x3FCC, 0x3FCE, 0x3FD0, 0x3FD2,
    0x3FD4, 0x3FD5, 0x3FD7, 0x3FD9, 0x3FDB, 0x3FDC, 0x3FDE, 0x3FE0,
    0x3FE1, 0x3FE3, 0x3FE4, 0x3FE6, 0x3FE7, 0x3FE8, 0x3FEA, 0x3FEB,
    0x3FEC, 0x3FED, 0x3FEF, 0x3FF0, 0x3FF1, 0x3FF2, 0x3FF3, 0x3FF4,
    0x3FF5, 0x3FF6, 0x3FF7, 0x3FF7, 0x3FF8, 0x3FF9, 0x3FFA, 0x3FFA,
    0x3FFB, 0x3FFC, 0x3FFC, 0x3FFD, 0x3FFD, 0x3FFE, 0x3FFE, 0x3FFE,
    0x3FFF, 0x3FFF, 0x3FFF, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000,
    0x4000, 0x4000,
    // clang-format on
};

static const int32_t m_AtanBaseTable[8] = {
    +0x0000, -0x4000, -0xFFFF, +0xC000, -0x8000, +0x4000, +0x8000, -0xC000,
};

static const int16_t m_AtanAngleTable[0x800] = {
    // clang-format off
    0x0000, 0x0005, 0x000A, 0x000F, 0x0014, 0x0019, 0x001F, 0x0024,
    0x0029, 0x002E, 0x0033, 0x0038, 0x003D, 0x0042, 0x0047, 0x004C,
    0x0051, 0x0057, 0x005C, 0x0061, 0x0066, 0x006B, 0x0070, 0x0075,
    0x007A, 0x007F, 0x0084, 0x008A, 0x008F, 0x0094, 0x0099, 0x009E,
    0x00A3, 0x00A8, 0x00AD, 0x00B2, 0x00B7, 0x00BC, 0x00C2, 0x00C7,
    0x00CC, 0x00D1, 0x00D6, 0x00DB, 0x00E0, 0x00E5, 0x00EA, 0x00EF,
    0x00F4, 0x00FA, 0x00FF, 0x0104, 0x0109, 0x010E, 0x0113, 0x0118,
    0x011D, 0x0122, 0x0127, 0x012C, 0x0131, 0x0137, 0x013C, 0x0141,
    0x0146, 0x014B, 0x0150, 0x0155, 0x015A, 0x015F, 0x0164, 0x0169,
    0x016F, 0x0174, 0x0179, 0x017E, 0x0183, 0x0188, 0x018D, 0x0192,
    0x0197, 0x019C, 0x01A1, 0x01A6, 0x01AC, 0x01B1, 0x01B6, 0x01BB,
    0x01C0, 0x01C5, 0x01CA, 0x01CF, 0x01D4, 0x01D9, 0x01DE, 0x01E3,
    0x01E9, 0x01EE, 0x01F3, 0x01F8, 0x01FD, 0x0202, 0x0207, 0x020C,
    0x0211, 0x0216, 0x021B, 0x0220, 0x0226, 0x022B, 0x0230, 0x0235,
    0x023A, 0x023F, 0x0244, 0x0249, 0x024E, 0x0253, 0x0258, 0x025D,
    0x0262, 0x0268, 0x026D, 0x0272, 0x0277, 0x027C, 0x0281, 0x0286,
    0x028B, 0x0290, 0x0295, 0x029A, 0x029F, 0x02A4, 0x02A9, 0x02AF,
    0x02B4, 0x02B9, 0x02BE, 0x02C3, 0x02C8, 0x02CD, 0x02D2, 0x02D7,
    0x02DC, 0x02E1, 0x02E6, 0x02EB, 0x02F0, 0x02F6, 0x02FB, 0x0300,
    0x0305, 0x030A, 0x030F, 0x0314, 0x0319, 0x031E, 0x0323, 0x0328,
    0x032D, 0x0332, 0x0337, 0x033C, 0x0341, 0x0347, 0x034C, 0x0351,
    0x0356, 0x035B, 0x0360, 0x0365, 0x036A, 0x036F, 0x0374, 0x0379,
    0x037E, 0x0383, 0x0388, 0x038D, 0x0392, 0x0397, 0x039C, 0x03A2,
    0x03A7, 0x03AC, 0x03B1, 0x03B6, 0x03BB, 0x03C0, 0x03C5, 0x03CA,
    0x03CF, 0x03D4, 0x03D9, 0x03DE, 0x03E3, 0x03E8, 0x03ED, 0x03F2,
    0x03F7, 0x03FC, 0x0401, 0x0407, 0x040C, 0x0411, 0x0416, 0x041B,
    0x0420, 0x0425, 0x042A, 0x042F, 0x0434, 0x0439, 0x043E, 0x0443,
    0x0448, 0x044D, 0x0452, 0x0457, 0x045C, 0x0461, 0x0466, 0x046B,
    0x0470, 0x0475, 0x047A, 0x047F, 0x0484, 0x0489, 0x048E, 0x0494,
    0x0499, 0x049E, 0x04A3, 0x04A8, 0x04AD, 0x04B2, 0x04B7, 0x04BC,
    0x04C1, 0x04C6, 0x04CB, 0x04D0, 0x04D5, 0x04DA, 0x04DF, 0x04E4,
    0x04E9, 0x04EE, 0x04F3, 0x04F8, 0x04FD, 0x0502, 0x0507, 0x050C,
    0x0511, 0x0516, 0x051B, 0x0520, 0x0525, 0x052A, 0x052F, 0x0534,
    0x0539, 0x053E, 0x0543, 0x0548, 0x054D, 0x0552, 0x0557, 0x055C,
    0x0561, 0x0566, 0x056B, 0x0570, 0x0575, 0x057A, 0x057F, 0x0584,
    0x0589, 0x058E, 0x0593, 0x0598, 0x059D, 0x05A2, 0x05A7, 0x05AC,
    0x05B1, 0x05B6, 0x05BB, 0x05C0, 0x05C5, 0x05CA, 0x05CF, 0x05D4,
    0x05D9, 0x05DE, 0x05E3, 0x05E8, 0x05ED, 0x05F2, 0x05F7, 0x05FC,
    0x0601, 0x0606, 0x060B, 0x0610, 0x0615, 0x061A, 0x061F, 0x0624,
    0x0629, 0x062E, 0x0633, 0x0638, 0x063D, 0x0642, 0x0647, 0x064C,
    0x0651, 0x0656, 0x065B, 0x0660, 0x0665, 0x066A, 0x066E, 0x0673,
    0x0678, 0x067D, 0x0682, 0x0687, 0x068C, 0x0691, 0x0696, 0x069B,
    0x06A0, 0x06A5, 0x06AA, 0x06AF, 0x06B4, 0x06B9, 0x06BE, 0x06C3,
    0x06C8, 0x06CD, 0x06D2, 0x06D7, 0x06DC, 0x06E1, 0x06E5, 0x06EA,
    0x06EF, 0x06F4, 0x06F9, 0x06FE, 0x0703, 0x0708, 0x070D, 0x0712,
    0x0717, 0x071C, 0x0721, 0x0726, 0x072B, 0x0730, 0x0735, 0x0739,
    0x073E, 0x0743, 0x0748, 0x074D, 0x0752, 0x0757, 0x075C, 0x0761,
    0x0766, 0x076B, 0x0770, 0x0775, 0x077A, 0x077E, 0x0783, 0x0788,
    0x078D, 0x0792, 0x0797, 0x079C, 0x07A1, 0x07A6, 0x07AB, 0x07B0,
    0x07B5, 0x07B9, 0x07BE, 0x07C3, 0x07C8, 0x07CD, 0x07D2, 0x07D7,
    0x07DC, 0x07E1, 0x07E6, 0x07EB, 0x07EF, 0x07F4, 0x07F9, 0x07FE,
    0x0803, 0x0808, 0x080D, 0x0812, 0x0817, 0x081C, 0x0820, 0x0825,
    0x082A, 0x082F, 0x0834, 0x0839, 0x083E, 0x0843, 0x0848, 0x084C,
    0x0851, 0x0856, 0x085B, 0x0860, 0x0865, 0x086A, 0x086F, 0x0873,
    0x0878, 0x087D, 0x0882, 0x0887, 0x088C, 0x0891, 0x0896, 0x089A,
    0x089F, 0x08A4, 0x08A9, 0x08AE, 0x08B3, 0x08B8, 0x08BD, 0x08C1,
    0x08C6, 0x08CB, 0x08D0, 0x08D5, 0x08DA, 0x08DF, 0x08E3, 0x08E8,
    0x08ED, 0x08F2, 0x08F7, 0x08FC, 0x0901, 0x0905, 0x090A, 0x090F,
    0x0914, 0x0919, 0x091E, 0x0922, 0x0927, 0x092C, 0x0931, 0x0936,
    0x093B, 0x093F, 0x0944, 0x0949, 0x094E, 0x0953, 0x0958, 0x095C,
    0x0961, 0x0966, 0x096B, 0x0970, 0x0975, 0x0979, 0x097E, 0x0983,
    0x0988, 0x098D, 0x0992, 0x0996, 0x099B, 0x09A0, 0x09A5, 0x09AA,
    0x09AE, 0x09B3, 0x09B8, 0x09BD, 0x09C2, 0x09C6, 0x09CB, 0x09D0,
    0x09D5, 0x09DA, 0x09DE, 0x09E3, 0x09E8, 0x09ED, 0x09F2, 0x09F6,
    0x09FB, 0x0A00, 0x0A05, 0x0A0A, 0x0A0E, 0x0A13, 0x0A18, 0x0A1D,
    0x0A22, 0x0A26, 0x0A2B, 0x0A30, 0x0A35, 0x0A39, 0x0A3E, 0x0A43,
    0x0A48, 0x0A4D, 0x0A51, 0x0A56, 0x0A5B, 0x0A60, 0x0A64, 0x0A69,
    0x0A6E, 0x0A73, 0x0A77, 0x0A7C, 0x0A81, 0x0A86, 0x0A8B, 0x0A8F,
    0x0A94, 0x0A99, 0x0A9E, 0x0AA2, 0x0AA7, 0x0AAC, 0x0AB1, 0x0AB5,
    0x0ABA, 0x0ABF, 0x0AC4, 0x0AC8, 0x0ACD, 0x0AD2, 0x0AD7, 0x0ADB,
    0x0AE0, 0x0AE5, 0x0AE9, 0x0AEE, 0x0AF3, 0x0AF8, 0x0AFC, 0x0B01,
    0x0B06, 0x0B0B, 0x0B0F, 0x0B14, 0x0B19, 0x0B1E, 0x0B22, 0x0B27,
    0x0B2C, 0x0B30, 0x0B35, 0x0B3A, 0x0B3F, 0x0B43, 0x0B48, 0x0B4D,
    0x0B51, 0x0B56, 0x0B5B, 0x0B60, 0x0B64, 0x0B69, 0x0B6E, 0x0B72,
    0x0B77, 0x0B7C, 0x0B80, 0x0B85, 0x0B8A, 0x0B8F, 0x0B93, 0x0B98,
    0x0B9D, 0x0BA1, 0x0BA6, 0x0BAB, 0x0BAF, 0x0BB4, 0x0BB9, 0x0BBD,
    0x0BC2, 0x0BC7, 0x0BCB, 0x0BD0, 0x0BD5, 0x0BD9, 0x0BDE, 0x0BE3,
    0x0BE7, 0x0BEC, 0x0BF1, 0x0BF5, 0x0BFA, 0x0BFF, 0x0C03, 0x0C08,
    0x0C0D, 0x0C11, 0x0C16, 0x0C1B, 0x0C1F, 0x0C24, 0x0C29, 0x0C2D,
    0x0C32, 0x0C37, 0x0C3B, 0x0C40, 0x0C45, 0x0C49, 0x0C4E, 0x0C53,
    0x0C57, 0x0C5C, 0x0C60, 0x0C65, 0x0C6A, 0x0C6E, 0x0C73, 0x0C78,
    0x0C7C, 0x0C81, 0x0C86, 0x0C8A, 0x0C8F, 0x0C93, 0x0C98, 0x0C9D,
    0x0CA1, 0x0CA6, 0x0CAB, 0x0CAF, 0x0CB4, 0x0CB8, 0x0CBD, 0x0CC2,
    0x0CC6, 0x0CCB, 0x0CCF, 0x0CD4, 0x0CD9, 0x0CDD, 0x0CE2, 0x0CE6,
    0x0CEB, 0x0CF0, 0x0CF4, 0x0CF9, 0x0CFD, 0x0D02, 0x0D07, 0x0D0B,
    0x0D10, 0x0D14, 0x0D19, 0x0D1E, 0x0D22, 0x0D27, 0x0D2B, 0x0D30,
    0x0D34, 0x0D39, 0x0D3E, 0x0D42, 0x0D47, 0x0D4B, 0x0D50, 0x0D54,
    0x0D59, 0x0D5E, 0x0D62, 0x0D67, 0x0D6B, 0x0D70, 0x0D74, 0x0D79,
    0x0D7D, 0x0D82, 0x0D87, 0x0D8B, 0x0D90, 0x0D94, 0x0D99, 0x0D9D,
    0x0DA2, 0x0DA6, 0x0DAB, 0x0DAF, 0x0DB4, 0x0DB9, 0x0DBD, 0x0DC2,
    0x0DC6, 0x0DCB, 0x0DCF, 0x0DD4, 0x0DD8, 0x0DDD, 0x0DE1, 0x0DE6,
    0x0DEA, 0x0DEF, 0x0DF3, 0x0DF8, 0x0DFC, 0x0E01, 0x0E05, 0x0E0A,
    0x0E0F, 0x0E13, 0x0E18, 0x0E1C, 0x0E21, 0x0E25, 0x0E2A, 0x0E2E,
    0x0E33, 0x0E37, 0x0E3C, 0x0E40, 0x0E45, 0x0E49, 0x0E4E, 0x0E52,
    0x0E56, 0x0E5B, 0x0E5F, 0x0E64, 0x0E68, 0x0E6D, 0x0E71, 0x0E76,
    0x0E7A, 0x0E7F, 0x0E83, 0x0E88, 0x0E8C, 0x0E91, 0x0E95, 0x0E9A,
    0x0E9E, 0x0EA3, 0x0EA7, 0x0EAC, 0x0EB0, 0x0EB4, 0x0EB9, 0x0EBD,
    0x0EC2, 0x0EC6, 0x0ECB, 0x0ECF, 0x0ED4, 0x0ED8, 0x0EDC, 0x0EE1,
    0x0EE5, 0x0EEA, 0x0EEE, 0x0EF3, 0x0EF7, 0x0EFC, 0x0F00, 0x0F04,
    0x0F09, 0x0F0D, 0x0F12, 0x0F16, 0x0F1B, 0x0F1F, 0x0F23, 0x0F28,
    0x0F2C, 0x0F31, 0x0F35, 0x0F3A, 0x0F3E, 0x0F42, 0x0F47, 0x0F4B,
    0x0F50, 0x0F54, 0x0F58, 0x0F5D, 0x0F61, 0x0F66, 0x0F6A, 0x0F6E,
    0x0F73, 0x0F77, 0x0F7C, 0x0F80, 0x0F84, 0x0F89, 0x0F8D, 0x0F91,
    0x0F96, 0x0F9A, 0x0F9F, 0x0FA3, 0x0FA7, 0x0FAC, 0x0FB0, 0x0FB5,
    0x0FB9, 0x0FBD, 0x0FC2, 0x0FC6, 0x0FCA, 0x0FCF, 0x0FD3, 0x0FD7,
    0x0FDC, 0x0FE0, 0x0FE5, 0x0FE9, 0x0FED, 0x0FF2, 0x0FF6, 0x0FFA,
    0x0FFF, 0x1003, 0x1007, 0x100C, 0x1010, 0x1014, 0x1019, 0x101D,
    0x1021, 0x1026, 0x102A, 0x102E, 0x1033, 0x1037, 0x103B, 0x1040,
    0x1044, 0x1048, 0x104D, 0x1051, 0x1055, 0x105A, 0x105E, 0x1062,
    0x1067, 0x106B, 0x106F, 0x1073, 0x1078, 0x107C, 0x1080, 0x1085,
    0x1089, 0x108D, 0x1092, 0x1096, 0x109A, 0x109E, 0x10A3, 0x10A7,
    0x10AB, 0x10B0, 0x10B4, 0x10B8, 0x10BC, 0x10C1, 0x10C5, 0x10C9,
    0x10CE, 0x10D2, 0x10D6, 0x10DA, 0x10DF, 0x10E3, 0x10E7, 0x10EB,
    0x10F0, 0x10F4, 0x10F8, 0x10FD, 0x1101, 0x1105, 0x1109, 0x110E,
    0x1112, 0x1116, 0x111A, 0x111F, 0x1123, 0x1127, 0x112B, 0x1130,
    0x1134, 0x1138, 0x113C, 0x1140, 0x1145, 0x1149, 0x114D, 0x1151,
    0x1156, 0x115A, 0x115E, 0x1162, 0x1166, 0x116B, 0x116F, 0x1173,
    0x1177, 0x117C, 0x1180, 0x1184, 0x1188, 0x118C, 0x1191, 0x1195,
    0x1199, 0x119D, 0x11A1, 0x11A6, 0x11AA, 0x11AE, 0x11B2, 0x11B6,
    0x11BB, 0x11BF, 0x11C3, 0x11C7, 0x11CB, 0x11CF, 0x11D4, 0x11D8,
    0x11DC, 0x11E0, 0x11E4, 0x11E9, 0x11ED, 0x11F1, 0x11F5, 0x11F9,
    0x11FD, 0x1202, 0x1206, 0x120A, 0x120E, 0x1212, 0x1216, 0x121A,
    0x121F, 0x1223, 0x1227, 0x122B, 0x122F, 0x1233, 0x1237, 0x123C,
    0x1240, 0x1244, 0x1248, 0x124C, 0x1250, 0x1254, 0x1259, 0x125D,
    0x1261, 0x1265, 0x1269, 0x126D, 0x1271, 0x1275, 0x127A, 0x127E,
    0x1282, 0x1286, 0x128A, 0x128E, 0x1292, 0x1296, 0x129A, 0x129F,
    0x12A3, 0x12A7, 0x12AB, 0x12AF, 0x12B3, 0x12B7, 0x12BB, 0x12BF,
    0x12C3, 0x12C7, 0x12CC, 0x12D0, 0x12D4, 0x12D8, 0x12DC, 0x12E0,
    0x12E4, 0x12E8, 0x12EC, 0x12F0, 0x12F4, 0x12F8, 0x12FC, 0x1301,
    0x1305, 0x1309, 0x130D, 0x1311, 0x1315, 0x1319, 0x131D, 0x1321,
    0x1325, 0x1329, 0x132D, 0x1331, 0x1335, 0x1339, 0x133D, 0x1341,
    0x1345, 0x1349, 0x134D, 0x1351, 0x1355, 0x135A, 0x135E, 0x1362,
    0x1366, 0x136A, 0x136E, 0x1372, 0x1376, 0x137A, 0x137E, 0x1382,
    0x1386, 0x138A, 0x138E, 0x1392, 0x1396, 0x139A, 0x139E, 0x13A2,
    0x13A6, 0x13AA, 0x13AE, 0x13B2, 0x13B6, 0x13BA, 0x13BE, 0x13C2,
    0x13C6, 0x13CA, 0x13CE, 0x13D2, 0x13D6, 0x13DA, 0x13DE, 0x13E2,
    0x13E6, 0x13E9, 0x13ED, 0x13F1, 0x13F5, 0x13F9, 0x13FD, 0x1401,
    0x1405, 0x1409, 0x140D, 0x1411, 0x1415, 0x1419, 0x141D, 0x1421,
    0x1425, 0x1429, 0x142D, 0x1431, 0x1435, 0x1439, 0x143D, 0x1440,
    0x1444, 0x1448, 0x144C, 0x1450, 0x1454, 0x1458, 0x145C, 0x1460,
    0x1464, 0x1468, 0x146C, 0x1470, 0x1473, 0x1477, 0x147B, 0x147F,
    0x1483, 0x1487, 0x148B, 0x148F, 0x1493, 0x1497, 0x149B, 0x149E,
    0x14A2, 0x14A6, 0x14AA, 0x14AE, 0x14B2, 0x14B6, 0x14BA, 0x14BE,
    0x14C1, 0x14C5, 0x14C9, 0x14CD, 0x14D1, 0x14D5, 0x14D9, 0x14DD,
    0x14E0, 0x14E4, 0x14E8, 0x14EC, 0x14F0, 0x14F4, 0x14F8, 0x14FB,
    0x14FF, 0x1503, 0x1507, 0x150B, 0x150F, 0x1513, 0x1516, 0x151A,
    0x151E, 0x1522, 0x1526, 0x152A, 0x152D, 0x1531, 0x1535, 0x1539,
    0x153D, 0x1541, 0x1544, 0x1548, 0x154C, 0x1550, 0x1554, 0x1558,
    0x155B, 0x155F, 0x1563, 0x1567, 0x156B, 0x156E, 0x1572, 0x1576,
    0x157A, 0x157E, 0x1581, 0x1585, 0x1589, 0x158D, 0x1591, 0x1594,
    0x1598, 0x159C, 0x15A0, 0x15A4, 0x15A7, 0x15AB, 0x15AF, 0x15B3,
    0x15B7, 0x15BA, 0x15BE, 0x15C2, 0x15C6, 0x15C9, 0x15CD, 0x15D1,
    0x15D5, 0x15D8, 0x15DC, 0x15E0, 0x15E4, 0x15E8, 0x15EB, 0x15EF,
    0x15F3, 0x15F7, 0x15FA, 0x15FE, 0x1602, 0x1606, 0x1609, 0x160D,
    0x1611, 0x1614, 0x1618, 0x161C, 0x1620, 0x1623, 0x1627, 0x162B,
    0x162F, 0x1632, 0x1636, 0x163A, 0x163E, 0x1641, 0x1645, 0x1649,
    0x164C, 0x1650, 0x1654, 0x1658, 0x165B, 0x165F, 0x1663, 0x1666,
    0x166A, 0x166E, 0x1671, 0x1675, 0x1679, 0x167D, 0x1680, 0x1684,
    0x1688, 0x168B, 0x168F, 0x1693, 0x1696, 0x169A, 0x169E, 0x16A1,
    0x16A5, 0x16A9, 0x16AC, 0x16B0, 0x16B4, 0x16B7, 0x16BB, 0x16BF,
    0x16C2, 0x16C6, 0x16CA, 0x16CD, 0x16D1, 0x16D5, 0x16D8, 0x16DC,
    0x16E0, 0x16E3, 0x16E7, 0x16EB, 0x16EE, 0x16F2, 0x16F6, 0x16F9,
    0x16FD, 0x1700, 0x1704, 0x1708, 0x170B, 0x170F, 0x1713, 0x1716,
    0x171A, 0x171D, 0x1721, 0x1725, 0x1728, 0x172C, 0x1730, 0x1733,
    0x1737, 0x173A, 0x173E, 0x1742, 0x1745, 0x1749, 0x174C, 0x1750,
    0x1754, 0x1757, 0x175B, 0x175E, 0x1762, 0x1766, 0x1769, 0x176D,
    0x1770, 0x1774, 0x1778, 0x177B, 0x177F, 0x1782, 0x1786, 0x1789,
    0x178D, 0x1791, 0x1794, 0x1798, 0x179B, 0x179F, 0x17A2, 0x17A6,
    0x17AA, 0x17AD, 0x17B1, 0x17B4, 0x17B8, 0x17BB, 0x17BF, 0x17C2,
    0x17C6, 0x17C9, 0x17CD, 0x17D1, 0x17D4, 0x17D8, 0x17DB, 0x17DF,
    0x17E2, 0x17E6, 0x17E9, 0x17ED, 0x17F0, 0x17F4, 0x17F7, 0x17FB,
    0x17FE, 0x1802, 0x1806, 0x1809, 0x180D, 0x1810, 0x1814, 0x1817,
    0x181B, 0x181E, 0x1822, 0x1825, 0x1829, 0x182C, 0x1830, 0x1833,
    0x1837, 0x183A, 0x183E, 0x1841, 0x1845, 0x1848, 0x184C, 0x184F,
    0x1853, 0x1856, 0x185A, 0x185D, 0x1860, 0x1864, 0x1867, 0x186B,
    0x186E, 0x1872, 0x1875, 0x1879, 0x187C, 0x1880, 0x1883, 0x1887,
    0x188A, 0x188E, 0x1891, 0x1894, 0x1898, 0x189B, 0x189F, 0x18A2,
    0x18A6, 0x18A9, 0x18AD, 0x18B0, 0x18B3, 0x18B7, 0x18BA, 0x18BE,
    0x18C1, 0x18C5, 0x18C8, 0x18CC, 0x18CF, 0x18D2, 0x18D6, 0x18D9,
    0x18DD, 0x18E0, 0x18E3, 0x18E7, 0x18EA, 0x18EE, 0x18F1, 0x18F5,
    0x18F8, 0x18FB, 0x18FF, 0x1902, 0x1906, 0x1909, 0x190C, 0x1910,
    0x1913, 0x1917, 0x191A, 0x191D, 0x1921, 0x1924, 0x1928, 0x192B,
    0x192E, 0x1932, 0x1935, 0x1938, 0x193C, 0x193F, 0x1943, 0x1946,
    0x1949, 0x194D, 0x1950, 0x1953, 0x1957, 0x195A, 0x195D, 0x1961,
    0x1964, 0x1968, 0x196B, 0x196E, 0x1972, 0x1975, 0x1978, 0x197C,
    0x197F, 0x1982, 0x1986, 0x1989, 0x198C, 0x1990, 0x1993, 0x1996,
    0x199A, 0x199D, 0x19A0, 0x19A4, 0x19A7, 0x19AA, 0x19AE, 0x19B1,
    0x19B4, 0x19B8, 0x19BB, 0x19BE, 0x19C2, 0x19C5, 0x19C8, 0x19CC,
    0x19CF, 0x19D2, 0x19D5, 0x19D9, 0x19DC, 0x19DF, 0x19E3, 0x19E6,
    0x19E9, 0x19ED, 0x19F0, 0x19F3, 0x19F6, 0x19FA, 0x19FD, 0x1A00,
    0x1A04, 0x1A07, 0x1A0A, 0x1A0D, 0x1A11, 0x1A14, 0x1A17, 0x1A1B,
    0x1A1E, 0x1A21, 0x1A24, 0x1A28, 0x1A2B, 0x1A2E, 0x1A31, 0x1A35,
    0x1A38, 0x1A3B, 0x1A3E, 0x1A42, 0x1A45, 0x1A48, 0x1A4B, 0x1A4F,
    0x1A52, 0x1A55, 0x1A58, 0x1A5C, 0x1A5F, 0x1A62, 0x1A65, 0x1A69,
    0x1A6C, 0x1A6F, 0x1A72, 0x1A76, 0x1A79, 0x1A7C, 0x1A7F, 0x1A83,
    0x1A86, 0x1A89, 0x1A8C, 0x1A8F, 0x1A93, 0x1A96, 0x1A99, 0x1A9C,
    0x1A9F, 0x1AA3, 0x1AA6, 0x1AA9, 0x1AAC, 0x1AB0, 0x1AB3, 0x1AB6,
    0x1AB9, 0x1ABC, 0x1AC0, 0x1AC3, 0x1AC6, 0x1AC9, 0x1ACC, 0x1ACF,
    0x1AD3, 0x1AD6, 0x1AD9, 0x1ADC, 0x1ADF, 0x1AE3, 0x1AE6, 0x1AE9,
    0x1AEC, 0x1AEF, 0x1AF2, 0x1AF6, 0x1AF9, 0x1AFC, 0x1AFF, 0x1B02,
    0x1B05, 0x1B09, 0x1B0C, 0x1B0F, 0x1B12, 0x1B15, 0x1B18, 0x1B1C,
    0x1B1F, 0x1B22, 0x1B25, 0x1B28, 0x1B2B, 0x1B2E, 0x1B32, 0x1B35,
    0x1B38, 0x1B3B, 0x1B3E, 0x1B41, 0x1B44, 0x1B48, 0x1B4B, 0x1B4E,
    0x1B51, 0x1B54, 0x1B57, 0x1B5A, 0x1B5D, 0x1B61, 0x1B64, 0x1B67,
    0x1B6A, 0x1B6D, 0x1B70, 0x1B73, 0x1B76, 0x1B79, 0x1B7D, 0x1B80,
    0x1B83, 0x1B86, 0x1B89, 0x1B8C, 0x1B8F, 0x1B92, 0x1B95, 0x1B98,
    0x1B9C, 0x1B9F, 0x1BA2, 0x1BA5, 0x1BA8, 0x1BAB, 0x1BAE, 0x1BB1,
    0x1BB4, 0x1BB7, 0x1BBA, 0x1BBD, 0x1BC1, 0x1BC4, 0x1BC7, 0x1BCA,
    0x1BCD, 0x1BD0, 0x1BD3, 0x1BD6, 0x1BD9, 0x1BDC, 0x1BDF, 0x1BE2,
    0x1BE5, 0x1BE8, 0x1BEB, 0x1BEE, 0x1BF2, 0x1BF5, 0x1BF8, 0x1BFB,
    0x1BFE, 0x1C01, 0x1C04, 0x1C07, 0x1C0A, 0x1C0D, 0x1C10, 0x1C13,
    0x1C16, 0x1C19, 0x1C1C, 0x1C1F, 0x1C22, 0x1C25, 0x1C28, 0x1C2B,
    0x1C2E, 0x1C31, 0x1C34, 0x1C37, 0x1C3A, 0x1C3D, 0x1C40, 0x1C43,
    0x1C46, 0x1C49, 0x1C4C, 0x1C4F, 0x1C52, 0x1C55, 0x1C58, 0x1C5B,
    0x1C5E, 0x1C61, 0x1C64, 0x1C67, 0x1C6A, 0x1C6D, 0x1C70, 0x1C73,
    0x1C76, 0x1C79, 0x1C7C, 0x1C7F, 0x1C82, 0x1C85, 0x1C88, 0x1C8B,
    0x1C8E, 0x1C91, 0x1C94, 0x1C97, 0x1C9A, 0x1C9D, 0x1CA0, 0x1CA3,
    0x1CA6, 0x1CA9, 0x1CAC, 0x1CAF, 0x1CB2, 0x1CB5, 0x1CB8, 0x1CBB,
    0x1CBE, 0x1CC1, 0x1CC3, 0x1CC6, 0x1CC9, 0x1CCC, 0x1CCF, 0x1CD2,
    0x1CD5, 0x1CD8, 0x1CDB, 0x1CDE, 0x1CE1, 0x1CE4, 0x1CE7, 0x1CEA,
    0x1CED, 0x1CF0, 0x1CF3, 0x1CF5, 0x1CF8, 0x1CFB, 0x1CFE, 0x1D01,
    0x1D04, 0x1D07, 0x1D0A, 0x1D0D, 0x1D10, 0x1D13, 0x1D16, 0x1D18,
    0x1D1B, 0x1D1E, 0x1D21, 0x1D24, 0x1D27, 0x1D2A, 0x1D2D, 0x1D30,
    0x1D33, 0x1D35, 0x1D38, 0x1D3B, 0x1D3E, 0x1D41, 0x1D44, 0x1D47,
    0x1D4A, 0x1D4D, 0x1D4F, 0x1D52, 0x1D55, 0x1D58, 0x1D5B, 0x1D5E,
    0x1D61, 0x1D64, 0x1D66, 0x1D69, 0x1D6C, 0x1D6F, 0x1D72, 0x1D75,
    0x1D78, 0x1D7B, 0x1D7D, 0x1D80, 0x1D83, 0x1D86, 0x1D89, 0x1D8C,
    0x1D8E, 0x1D91, 0x1D94, 0x1D97, 0x1D9A, 0x1D9D, 0x1DA0, 0x1DA2,
    0x1DA5, 0x1DA8, 0x1DAB, 0x1DAE, 0x1DB1, 0x1DB3, 0x1DB6, 0x1DB9,
    0x1DBC, 0x1DBF, 0x1DC2, 0x1DC4, 0x1DC7, 0x1DCA, 0x1DCD, 0x1DD0,
    0x1DD3, 0x1DD5, 0x1DD8, 0x1DDB, 0x1DDE, 0x1DE1, 0x1DE3, 0x1DE6,
    0x1DE9, 0x1DEC, 0x1DEF, 0x1DF1, 0x1DF4, 0x1DF7, 0x1DFA, 0x1DFD,
    0x1DFF, 0x1E02, 0x1E05, 0x1E08, 0x1E0B, 0x1E0D, 0x1E10, 0x1E13,
    0x1E16, 0x1E19, 0x1E1B, 0x1E1E, 0x1E21, 0x1E24, 0x1E26, 0x1E29,
    0x1E2C, 0x1E2F, 0x1E32, 0x1E34, 0x1E37, 0x1E3A, 0x1E3D, 0x1E3F,
    0x1E42, 0x1E45, 0x1E48, 0x1E4A, 0x1E4D, 0x1E50, 0x1E53, 0x1E55,
    0x1E58, 0x1E5B, 0x1E5E, 0x1E60, 0x1E63, 0x1E66, 0x1E69, 0x1E6B,
    0x1E6E, 0x1E71, 0x1E74, 0x1E76, 0x1E79, 0x1E7C, 0x1E7F, 0x1E81,
    0x1E84, 0x1E87, 0x1E8A, 0x1E8C, 0x1E8F, 0x1E92, 0x1E94, 0x1E97,
    0x1E9A, 0x1E9D, 0x1E9F, 0x1EA2, 0x1EA5, 0x1EA8, 0x1EAA, 0x1EAD,
    0x1EB0, 0x1EB2, 0x1EB5, 0x1EB8, 0x1EBA, 0x1EBD, 0x1EC0, 0x1EC3,
    0x1EC5, 0x1EC8, 0x1ECB, 0x1ECD, 0x1ED0, 0x1ED3, 0x1ED5, 0x1ED8,
    0x1EDB, 0x1EDE, 0x1EE0, 0x1EE3, 0x1EE6, 0x1EE8, 0x1EEB, 0x1EEE,
    0x1EF0, 0x1EF3, 0x1EF6, 0x1EF8, 0x1EFB, 0x1EFE, 0x1F00, 0x1F03,
    0x1F06, 0x1F08, 0x1F0B, 0x1F0E, 0x1F10, 0x1F13, 0x1F16, 0x1F18,
    0x1F1B, 0x1F1E, 0x1F20, 0x1F23, 0x1F26, 0x1F28, 0x1F2B, 0x1F2E,
    0x1F30, 0x1F33, 0x1F36, 0x1F38, 0x1F3B, 0x1F3D, 0x1F40, 0x1F43,
    0x1F45, 0x1F48, 0x1F4B, 0x1F4D, 0x1F50, 0x1F53, 0x1F55, 0x1F58,
    0x1F5A, 0x1F5D, 0x1F60, 0x1F62, 0x1F65, 0x1F68, 0x1F6A, 0x1F6D,
    0x1F6F, 0x1F72, 0x1F75, 0x1F77, 0x1F7A, 0x1F7C, 0x1F7F, 0x1F82,
    0x1F84, 0x1F87, 0x1F8A, 0x1F8C, 0x1F8F, 0x1F91, 0x1F94, 0x1F97,
    0x1F99, 0x1F9C, 0x1F9E, 0x1FA1, 0x1FA4, 0x1FA6, 0x1FA9, 0x1FAB,
    0x1FAE, 0x1FB0, 0x1FB3, 0x1FB6, 0x1FB8, 0x1FBB, 0x1FBD, 0x1FC0,
    0x1FC3, 0x1FC5, 0x1FC8, 0x1FCA, 0x1FCD, 0x1FCF, 0x1FD2, 0x1FD5,
    0x1FD7, 0x1FDA, 0x1FDC, 0x1FDF, 0x1FE1, 0x1FE4, 0x1FE6, 0x1FE9,
    0x1FEC, 0x1FEE, 0x1FF1, 0x1FF3, 0x1FF6, 0x1FF8, 0x1FFB, 0x1FFD,
    // clang-format on
};

int32_t __fastcall Math_Cos(int32_t angle)
{
    return Math_Sin(angle + PHD_90);
}

int32_t __fastcall Math_Sin(int32_t angle)
{
    uint16_t sector = (uint16_t)angle & (PHD_180 - 1);
    if (sector > PHD_90) {
        sector = PHD_180 - sector;
    }
    int16_t result = m_SinTable[sector >> 4];
    if ((uint16_t)angle >= PHD_180) {
        result = -result;
    }
    return result;
}

int32_t __fastcall Math_Atan(int32_t x, int32_t y)
{
    if (x == 0 && y == 0) {
        return 0;
    }

    int32_t base = 0;
    if (x < 0) {
        base |= 4;
        x = -x;
    }

    if (y < 0) {
        base |= 2;
        y = -y;
    }

    if (y > x) {
        base |= 1;
        int32_t tmp = x;
        x = y;
        y = tmp;
    }

    return ABS(m_AtanBaseTable[base] + m_AtanAngleTable[0x800 * y / x]);
}
