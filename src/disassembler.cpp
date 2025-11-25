/*
 * This file is part of Sandvik project.
 * Copyright (C) 2025 Christophe Duvernois
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "disassembler.hpp"

#include <fmt/format.h>

#include <stdexcept>

namespace {
	// Byte code value to identifier mapping
	constexpr const char* opcode_name[256] = {
	    "nop",                    /* 0x00 */
	    "move",                   /* 0x01 */
	    "move/from16",            /* 0x02 */
	    "move/16",                /* 0x03 */
	    "move-wide",              /* 0x04 */
	    "move-wide/from16",       /* 0x05 */
	    "move-wide/16",           /* 0x06 */
	    "move-object",            /* 0x07 */
	    "move-object/from16",     /* 0x08 */
	    "move-object/16",         /* 0x09 */
	    "move-result",            /* 0x0a */
	    "move-result-wide",       /* 0x0b */
	    "move-result-object",     /* 0x0c */
	    "move-exception",         /* 0x0d */
	    "return-void",            /* 0x0e */
	    "return",                 /* 0x0f */
	    "return-wide",            /* 0x10 */
	    "return-object",          /* 0x11 */
	    "const/4",                /* 0x12 */
	    "const/16",               /* 0x13 */
	    "const",                  /* 0x14 */
	    "const/high16",           /* 0x15 */
	    "const-wide/16",          /* 0x16 */
	    "const-wide/32",          /* 0x17 */
	    "const-wide",             /* 0x18 */
	    "const-wide/high16",      /* 0x19 */
	    "const-string",           /* 0x1a */
	    "const-string/jumbo",     /* 0x1b */
	    "const-class",            /* 0x1c */
	    "monitor-enter",          /* 0x1d */
	    "monitor-exit",           /* 0x1e */
	    "check-cast",             /* 0x1f */
	    "instance-of",            /* 0x20 */
	    "array-length",           /* 0x21 */
	    "new-instance",           /* 0x22 */
	    "new-array",              /* 0x23 */
	    "filled-new-array",       /* 0x24 */
	    "filled-new-array/range", /* 0x25 */
	    "fill-array-data",        /* 0x26 */
	    "throw",                  /* 0x27 */
	    "goto",                   /* 0x28 */
	    "goto/16",                /* 0x29 */
	    "goto/32",                /* 0x2a */
	    "packed-switch",          /* 0x2b */
	    "sparse-switch",          /* 0x2c */
	    "cmpl-float",             /* 0x2d */
	    "cmpg-float",             /* 0x2e */
	    "cmpl-double",            /* 0x2f */
	    "cmpg-double",            /* 0x30 */
	    "cmp-long",               /* 0x31 */
	    "if-eq",                  /* 0x32 */
	    "if-ne",                  /* 0x33 */
	    "if-lt",                  /* 0x34 */
	    "if-ge",                  /* 0x35 */
	    "if-gt",                  /* 0x36 */
	    "if-le",                  /* 0x37 */
	    "if-eqz",                 /* 0x38 */
	    "if-nez",                 /* 0x39 */
	    "if-ltz",                 /* 0x3a */
	    "if-gez",                 /* 0x3b */
	    "if-gtz",                 /* 0x3c */
	    "if-lez",                 /* 0x3d */
	    nullptr,                  /* 0x3e */
	    nullptr,                  /* 0x3f */
	    nullptr,                  /* 0x40 */
	    nullptr,                  /* 0x41 */
	    nullptr,                  /* 0x42 */
	    nullptr,                  /* 0x43 */
	    "aget",                   /* 0x44 */
	    "aget-wide",              /* 0x45 */
	    "aget-object",            /* 0x46 */
	    "aget-boolean",           /* 0x47 */
	    "aget-byte",              /* 0x48 */
	    "aget-char",              /* 0x49 */
	    "aget-short",             /* 0x4a */
	    "aput",                   /* 0x4b */
	    "aput-wide",              /* 0x4c */
	    "aput-object",            /* 0x4d */
	    "aput-boolean",           /* 0x4e */
	    "aput-byte",              /* 0x4f */
	    "aput-char",              /* 0x50 */
	    "aput-short",             /* 0x51 */
	    "iget",                   /* 0x52 */
	    "iget-wide",              /* 0x53 */
	    "iget-object",            /* 0x54 */
	    "iget-boolean",           /* 0x55 */
	    "iget-byte",              /* 0x56 */
	    "iget-char",              /* 0x57 */
	    "iget-short",             /* 0x58 */
	    "iput",                   /* 0x59 */
	    "iput-wide",              /* 0x5a */
	    "iput-object",            /* 0x5b */
	    "iput-boolean",           /* 0x5c */
	    "iput-byte",              /* 0x5d */
	    "iput-char",              /* 0x5e */
	    "iput-short",             /* 0x5f */
	    "sget",                   /* 0x60 */
	    "sget-wide",              /* 0x61 */
	    "sget-object",            /* 0x62 */
	    "sget-boolean",           /* 0x63 */
	    "sget-byte",              /* 0x64 */
	    "sget-char",              /* 0x65 */
	    "sget-short",             /* 0x66 */
	    "sput",                   /* 0x67 */
	    "sput-wide",              /* 0x68 */
	    "sput-object",            /* 0x69 */
	    "sput-boolean",           /* 0x6a */
	    "sput-byte",              /* 0x6b */
	    "sput-char",              /* 0x6c */
	    "sput-short",             /* 0x6d */
	    "invoke-virtual",         /* 0x6e */
	    "invoke-super",           /* 0x6f */
	    "invoke-direct",          /* 0x70 */
	    "invoke-static",          /* 0x71 */
	    "invoke-interface",       /* 0x72 */
	    nullptr,                  /* 0x73 */
	    "invoke-virtual/range",   /* 0x74 */
	    "invoke-super/range",     /* 0x75 */
	    "invoke-direct/range",    /* 0x76 */
	    "invoke-static/range",    /* 0x77 */
	    "invoke-interface/range", /* 0x78 */
	    nullptr,                  /* 0x79 */
	    nullptr,                  /* 0x7a */
	    "neg-int",                /* 0x7b */
	    "not-int",                /* 0x7c */
	    "neg-long",               /* 0x7d */
	    "not-long",               /* 0x7e */
	    "neg-float",              /* 0x7f */
	    "neg-double",             /* 0x80 */
	    "int-to-long",            /* 0x81 */
	    "int-to-float",           /* 0x82 */
	    "int-to-double",          /* 0x83 */
	    "long-to-int",            /* 0x84 */
	    "long-to-float",          /* 0x85 */
	    "long-to-double",         /* 0x86 */
	    "float-to-int",           /* 0x87 */
	    "float-to-long",          /* 0x88 */
	    "float-to-double",        /* 0x89 */
	    "double-to-int",          /* 0x8a */
	    "double-to-long",         /* 0x8b */
	    "double-to-float",        /* 0x8c */
	    "int-to-byte",            /* 0x8d */
	    "int-to-char",            /* 0x8e */
	    "int-to-short",           /* 0x8f */
	    "add-int",                /* 0x90 */
	    "sub-int",                /* 0x91 */
	    "mul-int",                /* 0x92 */
	    "div-int",                /* 0x93 */
	    "rem-int",                /* 0x94 */
	    "and-int",                /* 0x95 */
	    "or-int",                 /* 0x96 */
	    "xor-int",                /* 0x97 */
	    "shl-int",                /* 0x98 */
	    "shr-int",                /* 0x99 */
	    "ushr-int",               /* 0x9a */
	    "add-long",               /* 0x9b */
	    "sub-long",               /* 0x9c */
	    "mul-long",               /* 0x9d */
	    "div-long",               /* 0x9e */
	    "rem-long",               /* 0x9f */
	    "and-long",               /* 0xa0 */
	    "or-long",                /* 0xa1 */
	    "xor-long",               /* 0xa2 */
	    "shl-long",               /* 0xa3 */
	    "shr-long",               /* 0xa4 */
	    "ushr-long",              /* 0xa5 */
	    "add-float",              /* 0xa6 */
	    "sub-float",              /* 0xa7 */
	    "mul-float",              /* 0xa8 */
	    "div-float",              /* 0xa9 */
	    "rem-float",              /* 0xaa */
	    "add-double",             /* 0xab */
	    "sub-double",             /* 0xac */
	    "mul-double",             /* 0xad */
	    "div-double",             /* 0xae */
	    "rem-double",             /* 0xaf */
	    "add-int/2addr",          /* 0xb0 */
	    "sub-int/2addr",          /* 0xb1 */
	    "mul-int/2addr",          /* 0xb2 */
	    "div-int/2addr",          /* 0xb3 */
	    "rem-int/2addr",          /* 0xb4 */
	    "and-int/2addr",          /* 0xb5 */
	    "or-int/2addr",           /* 0xb6 */
	    "xor-int/2addr",          /* 0xb7 */
	    "shl-int/2addr",          /* 0xb8 */
	    "shr-int/2addr",          /* 0xb9 */
	    "ushr-int/2addr",         /* 0xba */
	    "add-long/2addr",         /* 0xbb */
	    "sub-long/2addr",         /* 0xbc */
	    "mul-long/2addr",         /* 0xbd */
	    "div-long/2addr",         /* 0xbe */
	    "rem-long/2addr",         /* 0xbf */
	    "and-long/2addr",         /* 0xc0 */
	    "or-long/2addr",          /* 0xc1 */
	    "xor-long/2addr",         /* 0xc2 */
	    "shl-long/2addr",         /* 0xc3 */
	    "shr-long/2addr",         /* 0xc4 */
	    "ushr-long/2addr",        /* 0xc5 */
	    "add-float/2addr",        /* 0xc6 */
	    "sub-float/2addr",        /* 0xc7 */
	    "mul-float/2addr",        /* 0xc8 */
	    "div-float/2addr",        /* 0xc9 */
	    "rem-float/2addr",        /* 0xca */
	    "add-double/2addr",       /* 0xcb */
	    "sub-double/2addr",       /* 0xcc */
	    "mul-double/2addr",       /* 0xcd */
	    "div-double/2addr",       /* 0xce */
	    "rem-double/2addr",       /* 0xcf */
	    "add-int/lit16",          /* 0xd0 */
	    "rsub-int",               /* 0xd1 */
	    "mul-int/lit16",          /* 0xd2 */
	    "div-int/lit16",          /* 0xd3 */
	    "rem-int/lit16",          /* 0xd4 */
	    "and-int/lit16",          /* 0xd5 */
	    "or-int/lit16",           /* 0xd6 */
	    "xor-int/lit16",          /* 0xd7 */
	    "add-int/lit8",           /* 0xd8 */
	    "rsub-int/lit8",          /* 0xd9 */
	    "mul-int/lit8",           /* 0xda */
	    "div-int/lit8",           /* 0xdb */
	    "rem-int/lit8",           /* 0xdc */
	    "and-int/lit8",           /* 0xdd */
	    "or-int/lit8",            /* 0xde */
	    "xor-int/lit8",           /* 0xdf */
	    "shl-int/lit8",           /* 0xe0 */
	    "shr-int/lit8",           /* 0xe1 */
	    "ushr-int/lit8",          /* 0xe2 */
	    nullptr,                  /* 0xe3 */
	    nullptr,                  /* 0xe4 */
	    nullptr,                  /* 0xe5 */
	    nullptr,                  /* 0xe6 */
	    nullptr,                  /* 0xe7 */
	    nullptr,                  /* 0xe8 */
	    nullptr,                  /* 0xe9 */
	    nullptr,                  /* 0xea */
	    nullptr,                  /* 0xeb */
	    nullptr,                  /* 0xec */
	    nullptr,                  /* 0xed */
	    nullptr,                  /* 0xee */
	    nullptr,                  /* 0xef */
	    nullptr,                  /* 0xf0 */
	    nullptr,                  /* 0xf1 */
	    nullptr,                  /* 0xf2 */
	    nullptr,                  /* 0xf3 */
	    nullptr,                  /* 0xf4 */
	    nullptr,                  /* 0xf5 */
	    nullptr,                  /* 0xf6 */
	    nullptr,                  /* 0xf7 */
	    nullptr,                  /* 0xf8 */
	    nullptr,                  /* 0xf9 */
	    nullptr,                  /* 0xfa */
	    nullptr,                  /* 0xfb */
	    nullptr,                  /* 0xfc */
	    nullptr,                  /* 0xfd */
	    nullptr,                  /* 0xfe */
	    nullptr,                  /* 0xff */
	};
}  // namespace

using namespace sandvik;

Disassembler::Disassembler() {
	_dispatch.resize(256, [](const std::string&, const uint8_t*, uint32_t& size_) {
		size_ += 1;
		return "Unknown opcode";
	});

	_dispatch[0x00] = std::bind_front(&Disassembler::format_i10x, this);
	_dispatch[0x01] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0x02] = std::bind_front(&Disassembler::format_i22x, this);
	_dispatch[0x03] = std::bind_front(&Disassembler::format_i32x, this);
	_dispatch[0x04] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0x05] = std::bind_front(&Disassembler::format_i22x, this);
	_dispatch[0x06] = std::bind_front(&Disassembler::format_i32x, this);
	_dispatch[0x07] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0x08] = std::bind_front(&Disassembler::format_i22x, this);
	_dispatch[0x09] = std::bind_front(&Disassembler::format_i32x, this);
	_dispatch[0x0a] = std::bind_front(&Disassembler::format_i11x, this);
	_dispatch[0x0b] = std::bind_front(&Disassembler::format_i11x, this);
	_dispatch[0x0c] = std::bind_front(&Disassembler::format_i11x, this);
	_dispatch[0x0d] = std::bind_front(&Disassembler::format_i11x, this);
	_dispatch[0x0e] = std::bind_front(&Disassembler::format_i10x, this);
	_dispatch[0x0f] = std::bind_front(&Disassembler::format_i11x, this);
	_dispatch[0x10] = std::bind_front(&Disassembler::format_i11x, this);
	_dispatch[0x11] = std::bind_front(&Disassembler::format_i11x, this);
	_dispatch[0x12] = std::bind_front(&Disassembler::format_i11n, this);
	_dispatch[0x13] = std::bind_front(&Disassembler::format_i21s, this);
	_dispatch[0x14] = std::bind_front(&Disassembler::format_i31i, this);
	_dispatch[0x15] = std::bind_front(&Disassembler::format_i21h, this);
	_dispatch[0x16] = std::bind_front(&Disassembler::format_i21s, this);
	_dispatch[0x17] = std::bind_front(&Disassembler::format_i31i, this);
	_dispatch[0x18] = std::bind_front(&Disassembler::format_i51l, this);
	_dispatch[0x19] = std::bind_front(&Disassembler::format_i21h, this);
	_dispatch[0x1a] = std::bind_front(&Disassembler::format_i21c, this);
	_dispatch[0x1b] = std::bind_front(&Disassembler::format_i31c, this);
	_dispatch[0x1c] = std::bind_front(&Disassembler::format_i21c, this);
	_dispatch[0x1d] = std::bind_front(&Disassembler::format_i11x, this);
	_dispatch[0x1e] = std::bind_front(&Disassembler::format_i11x, this);
	_dispatch[0x1f] = std::bind_front(&Disassembler::format_i21c, this);
	_dispatch[0x20] = std::bind_front(&Disassembler::format_i22c, this);
	_dispatch[0x21] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0x22] = std::bind_front(&Disassembler::format_i21c, this);
	_dispatch[0x23] = std::bind_front(&Disassembler::format_i22c, this);
	_dispatch[0x24] = std::bind_front(&Disassembler::format_i25c, this);
	_dispatch[0x25] = std::bind_front(&Disassembler::format_i3rc, this);
	_dispatch[0x26] = std::bind_front(&Disassembler::format_i31t, this);
	_dispatch[0x27] = std::bind_front(&Disassembler::format_i11x, this);
	_dispatch[0x28] = std::bind_front(&Disassembler::format_i10t, this);
	_dispatch[0x29] = std::bind_front(&Disassembler::format_i20t, this);
	_dispatch[0x2a] = std::bind_front(&Disassembler::format_i30t, this);
	_dispatch[0x2b] = std::bind_front(&Disassembler::format_i31t, this);
	_dispatch[0x2c] = std::bind_front(&Disassembler::format_i31t, this);
	_dispatch[0x2d] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0x2e] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0x2f] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0x30] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0x31] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0x32] = std::bind_front(&Disassembler::format_i22t, this);
	_dispatch[0x33] = std::bind_front(&Disassembler::format_i22t, this);
	_dispatch[0x34] = std::bind_front(&Disassembler::format_i22t, this);
	_dispatch[0x35] = std::bind_front(&Disassembler::format_i22t, this);
	_dispatch[0x36] = std::bind_front(&Disassembler::format_i22t, this);
	_dispatch[0x37] = std::bind_front(&Disassembler::format_i22t, this);
	_dispatch[0x38] = std::bind_front(&Disassembler::format_i21t, this);
	_dispatch[0x39] = std::bind_front(&Disassembler::format_i21t, this);
	_dispatch[0x3a] = std::bind_front(&Disassembler::format_i21t, this);
	_dispatch[0x3b] = std::bind_front(&Disassembler::format_i21t, this);
	_dispatch[0x3c] = std::bind_front(&Disassembler::format_i21t, this);
	_dispatch[0x3d] = std::bind_front(&Disassembler::format_i21t, this);
	// 0x3E ... 0x43 (unused)
	_dispatch[0x44] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0x45] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0x46] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0x47] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0x48] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0x49] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0x4a] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0x4b] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0x4c] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0x4d] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0x4e] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0x4f] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0x50] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0x51] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0x52] = std::bind_front(&Disassembler::format_i22c, this);
	_dispatch[0x53] = std::bind_front(&Disassembler::format_i22c, this);
	_dispatch[0x54] = std::bind_front(&Disassembler::format_i22c, this);
	_dispatch[0x55] = std::bind_front(&Disassembler::format_i22c, this);
	_dispatch[0x56] = std::bind_front(&Disassembler::format_i22c, this);
	_dispatch[0x57] = std::bind_front(&Disassembler::format_i22c, this);
	_dispatch[0x58] = std::bind_front(&Disassembler::format_i22c, this);
	_dispatch[0x59] = std::bind_front(&Disassembler::format_i22c, this);
	_dispatch[0x5a] = std::bind_front(&Disassembler::format_i22c, this);
	_dispatch[0x5b] = std::bind_front(&Disassembler::format_i22c, this);
	_dispatch[0x5c] = std::bind_front(&Disassembler::format_i22c, this);
	_dispatch[0x5d] = std::bind_front(&Disassembler::format_i22c, this);
	_dispatch[0x5e] = std::bind_front(&Disassembler::format_i22c, this);
	_dispatch[0x5f] = std::bind_front(&Disassembler::format_i22c, this);
	_dispatch[0x60] = std::bind_front(&Disassembler::format_i21c, this);
	_dispatch[0x61] = std::bind_front(&Disassembler::format_i21c, this);
	_dispatch[0x62] = std::bind_front(&Disassembler::format_i21c, this);
	_dispatch[0x63] = std::bind_front(&Disassembler::format_i21c, this);
	_dispatch[0x64] = std::bind_front(&Disassembler::format_i21c, this);
	_dispatch[0x65] = std::bind_front(&Disassembler::format_i21c, this);
	_dispatch[0x66] = std::bind_front(&Disassembler::format_i21c, this);
	_dispatch[0x67] = std::bind_front(&Disassembler::format_i21c, this);
	_dispatch[0x68] = std::bind_front(&Disassembler::format_i21c, this);
	_dispatch[0x69] = std::bind_front(&Disassembler::format_i21c, this);
	_dispatch[0x6a] = std::bind_front(&Disassembler::format_i21c, this);
	_dispatch[0x6b] = std::bind_front(&Disassembler::format_i21c, this);
	_dispatch[0x6c] = std::bind_front(&Disassembler::format_i21c, this);
	_dispatch[0x6d] = std::bind_front(&Disassembler::format_i21c, this);
	_dispatch[0x6e] = std::bind_front(&Disassembler::format_i35c, this);
	_dispatch[0x6f] = std::bind_front(&Disassembler::format_i35c, this);
	_dispatch[0x70] = std::bind_front(&Disassembler::format_i35c, this);
	_dispatch[0x71] = std::bind_front(&Disassembler::format_i35c, this);
	_dispatch[0x72] = std::bind_front(&Disassembler::format_i35c, this);
	// 73 unused
	_dispatch[0x74] = std::bind_front(&Disassembler::format_i3rc, this);
	_dispatch[0x75] = std::bind_front(&Disassembler::format_i3rc, this);
	_dispatch[0x76] = std::bind_front(&Disassembler::format_i3rc, this);
	_dispatch[0x77] = std::bind_front(&Disassembler::format_i3rc, this);
	_dispatch[0x78] = std::bind_front(&Disassembler::format_i3rc, this);
	// 0x79 ... 0x7A (unused)
	_dispatch[0x7b] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0x7c] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0x7d] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0x7e] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0x7f] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0x80] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0x81] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0x82] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0x83] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0x84] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0x85] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0x86] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0x87] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0x88] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0x89] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0x8a] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0x8b] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0x8c] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0x8d] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0x8e] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0x8f] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0x90] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0x91] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0x92] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0x93] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0x94] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0x95] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0x96] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0x97] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0x98] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0x99] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0x9a] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0x9b] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0x9c] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0x9d] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0x9e] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0x9f] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0xa0] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0xa1] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0xa2] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0xa3] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0xa4] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0xa5] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0xa6] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0xa7] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0xa8] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0xa9] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0xaa] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0xab] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0xac] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0xad] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0xae] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0xaf] = std::bind_front(&Disassembler::format_i23x, this);
	_dispatch[0xb0] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0xb1] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0xb2] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0xb3] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0xb4] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0xb5] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0xb6] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0xb7] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0xb8] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0xb9] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0xba] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0xbb] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0xbc] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0xbd] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0xbe] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0xbf] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0xc0] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0xc1] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0xc2] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0xc3] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0xc4] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0xc5] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0xc6] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0xc7] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0xc8] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0xc9] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0xca] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0xcb] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0xcc] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0xcd] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0xce] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0xcf] = std::bind_front(&Disassembler::format_i12x, this);
	_dispatch[0xd0] = std::bind_front(&Disassembler::format_i22s, this);
	_dispatch[0xd1] = std::bind_front(&Disassembler::format_i22s, this);
	_dispatch[0xd2] = std::bind_front(&Disassembler::format_i22s, this);
	_dispatch[0xd3] = std::bind_front(&Disassembler::format_i22s, this);
	_dispatch[0xd4] = std::bind_front(&Disassembler::format_i22s, this);
	_dispatch[0xd5] = std::bind_front(&Disassembler::format_i22s, this);
	_dispatch[0xd6] = std::bind_front(&Disassembler::format_i22s, this);
	_dispatch[0xd7] = std::bind_front(&Disassembler::format_i22s, this);
	_dispatch[0xd8] = std::bind_front(&Disassembler::format_i22b, this);
	_dispatch[0xd9] = std::bind_front(&Disassembler::format_i22b, this);
	_dispatch[0xda] = std::bind_front(&Disassembler::format_i22b, this);
	_dispatch[0xdb] = std::bind_front(&Disassembler::format_i22b, this);
	_dispatch[0xdc] = std::bind_front(&Disassembler::format_i22b, this);
	_dispatch[0xdd] = std::bind_front(&Disassembler::format_i22b, this);
	_dispatch[0xde] = std::bind_front(&Disassembler::format_i22b, this);
	_dispatch[0xdf] = std::bind_front(&Disassembler::format_i22b, this);
	_dispatch[0xe0] = std::bind_front(&Disassembler::format_i22b, this);
	_dispatch[0xe1] = std::bind_front(&Disassembler::format_i22b, this);
	_dispatch[0xe2] = std::bind_front(&Disassembler::format_i22b, this);
	// 0xE3 ... 0xFF (unused)
}

std::string Disassembler::disassemble(const uint8_t opcode_) const {
	auto name = opcode_name[opcode_];
	if (name != nullptr) {
		return std::string(name);
	}
	return "";
}

std::string Disassembler::disassemble(const uint8_t* bytecode_) const {
	uint32_t size = 0;
	return disassemble(bytecode_, size);
}

std::string Disassembler::disassemble(const uint8_t* bytecode_, uint32_t& size_) const {
	uint8_t opcode = bytecode_[0];
	size_ += 1;
	auto disasm = _dispatch[opcode](opcode_name[opcode], bytecode_ + 1, size_);
	std::string s = "";
	for (uint32_t i = 0; i < size_; ++i) {
		s += fmt::format(" {:02x}", bytecode_[i]);
	}
	return fmt::format("{:<40}:{}", disasm, s);
}

std::string Disassembler::format_i10x(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const {
	return fmt::format("{}", name_);
}

std::string Disassembler::format_i12x(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const {
	size_ += 1;
	return fmt::format("{} v{}, v{}", name_, operand_[0] & 0x0F, (operand_[0] >> 4) & 0x0F);
}

std::string Disassembler::format_i11n(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const {
	size_ += 1;
	auto value = static_cast<int8_t>((operand_[0] >> 4) & 0x0F);
	if (value & 0x08) {
		value |= 0xF0;  // Sign extend the 4-bit value to 8 bits
	}
	return fmt::format("{} v{}, #{}", name_, operand_[0] & 0x0F, value);
}

std::string Disassembler::format_i11x(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const {
	size_ += 1;
	return fmt::format("{} v{}", name_, operand_[0]);
}

std::string Disassembler::format_i10t(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const {
	size_ += 1;
	return fmt::format("{} +{:04x}", name_, operand_[0]);
}

std::string Disassembler::format_i20t(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const {
	// skip the first operand byte
	size_ += 3;
	int16_t AAAA = *(const int16_t*)&operand_[1];
	return fmt::format("{} {}", name_, AAAA);
}

std::string Disassembler::format_i22x(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const {
	size_ += 3;
	uint8_t vAA = operand_[0];
	uint16_t vBBBB = *(const uint16_t*)&operand_[1];
	return fmt::format("{} v{}, v{}", name_, vAA, vBBBB);
}

std::string Disassembler::format_i21t(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const {
	size_ += 2;
	uint8_t vAA = operand_[0];
	uint16_t vBBBB = *(const uint16_t*)&operand_[1];
	return fmt::format("{} v{}, {:04x} // +{:04x}", name_, vAA, vBBBB + 2, vBBBB);
}

std::string Disassembler::format_i21s(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const {
	size_ += 3;
	uint8_t vAA = operand_[0];
	int16_t offset = *(const int16_t*)&operand_[1];
	return fmt::format("{} v{}, #{}", name_, vAA, offset);
}

std::string Disassembler::format_i21h(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const {
	size_ += 3;
	uint8_t vAA = operand_[0];
	int32_t imm = *(const int16_t*)&operand_[1] << 16;
	return fmt::format("{} v{}, #{}", name_, vAA, imm);
}

std::string Disassembler::format_i21c(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const {
	size_ += 3;
	uint8_t vAA = operand_[0];
	uint16_t string_idx = *(const uint16_t*)&operand_[1];
	return fmt::format("{} v{}, string@{}", name_, vAA, string_idx);
}

std::string Disassembler::format_i23x(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const {
	size_ += 3;
	uint8_t vAA = operand_[0];
	uint8_t vBB = operand_[1];
	uint8_t vCC = operand_[2];
	return fmt::format("{} v{}, v{}, v{}", name_, vAA, vBB, vCC);
}

std::string Disassembler::format_i22b(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const {
	size_ += 3;
	uint8_t vAA = operand_[0];
	uint8_t vBB = operand_[1];
	uint8_t imm = operand_[2];
	return fmt::format("{} v{}, v{}, #{}", name_, vAA, vBB, imm);
}

std::string Disassembler::format_i22t(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const {
	size_ += 3;
	uint8_t vA = operand_[0] & 0x0F;
	uint8_t vB = (operand_[0] >> 4) & 0x0F;
	int16_t offset = *(const int16_t*)&operand_[1];
	return fmt::format("{} v{}, v{}, {:04x}", name_, vA, vB, offset);
}

std::string Disassembler::format_i22s(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const {
	size_ += 3;
	uint8_t vA = operand_[0] & 0x0F;
	uint8_t vB = (operand_[0] >> 4) & 0x0F;
	int16_t imm = *(const int16_t*)&operand_[1];
	return fmt::format("{} v{}, v{}, #{}", name_, vA, vB, imm);
}

std::string Disassembler::format_i22c(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const {
	size_ += 3;
	uint8_t vA = operand_[0] & 0x0F;
	uint8_t vB = (operand_[0] >> 4) & 0x0F;
	uint16_t type_idx = *(const uint16_t*)&operand_[1];
	return fmt::format("{} v{}, v{}, type@{}", name_, vA, vB, type_idx);
}

std::string Disassembler::format_i25c(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const {
	size_ += 5;
	uint8_t reg_count = (operand_[0] >> 4) & 0x0F;         // Number of registers used
	uint16_t method_idx = *(const uint16_t*)&operand_[1];  // Method index
	uint8_t vC = operand_[3] & 0x0F;
	uint8_t vD = (reg_count > 0) ? (operand_[3] >> 4) & 0x0F : 0;
	uint8_t vE = (reg_count > 1) ? operand_[4] & 0x0F : 0;
	uint8_t vF = (reg_count > 2) ? (operand_[4] >> 4) & 0x0F : 0;
	uint8_t vG = operand_[0] & 0x0F;

	switch (reg_count) {
		case 0:
			return fmt::format("{} type@{}", name_, method_idx);
		case 1:
			return fmt::format("{} {{v{}}}, type@{}", name_, vC, method_idx);
		case 2:
			return fmt::format("{} {{v{}, v{}}}, type@{}", name_, vC, vD, method_idx);
		case 3:
			return fmt::format("{} {{v{}, v{}, v{}}}, type@{}", name_, vC, vD, vE, method_idx);
		case 4:
			return fmt::format("{} {{v{}, v{}, v{}, v{}}}, type@{}", name_, vC, vD, vE, vF, method_idx);
		case 5:
			return fmt::format("{} {{v{}, v{}, v{}, v{}, v{}}}, type@{}", name_, vC, vD, vE, vF, vG, method_idx);
		default:
			return fmt::format("{} {{v{}, v{}, v{}, v{}, v{}}}, type@{} (unsupported reg count={})", name_, vC, vD, vE, vF, vG, method_idx, reg_count);
	}
}

std::string Disassembler::format_i30t(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const {
	size_ += 5;
	int32_t offset = *(const int32_t*)&operand_[1];
	return fmt::format("{} {}", name_, offset);
}

std::string Disassembler::format_i32x(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const {
	size_ += 4;
	uint16_t vAAAA = *(const uint16_t*)&operand_[0];
	uint16_t vBBBB = *(const uint16_t*)&operand_[2];
	return fmt::format("{} v{}, v{}", name_, vAAAA, vBBBB);
}

std::string Disassembler::format_i31i(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const {
	size_ += 5;
	uint8_t vAA = operand_[0];
	int32_t imm = *(const int32_t*)&operand_[1] & 0x00FFFFFF;
	return fmt::format("{} v{}, {}", name_, vAA, imm);
}

std::string Disassembler::format_i31t(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const {
	size_ += 5;
	uint8_t vAA = operand_[0];
	int32_t offset = *(const int32_t*)&operand_[1] & 0x00FFFFFF;
	return fmt::format("{} v{}, {}", name_, vAA, offset);
}

std::string Disassembler::format_i31c(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const {
	size_ += 5;
	uint8_t vAA = operand_[0];
	uint32_t string_idx = *(const uint32_t*)&operand_[1] & 0x00FFFFFF;
	return fmt::format("{} v{}, string@{}", name_, vAA, string_idx);
}

std::string Disassembler::format_i35c(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const {
	size_ += 5;
	uint8_t reg_count = (operand_[0] >> 4) & 0x0F;         // Number of registers used
	uint16_t method_idx = *(const uint16_t*)&operand_[1];  // Method index
	uint8_t vC = operand_[3] & 0x0F;
	uint8_t vD = (reg_count > 0) ? (operand_[3] >> 4) & 0x0F : 0;
	uint8_t vE = (reg_count > 1) ? operand_[4] & 0x0F : 0;
	uint8_t vF = (reg_count > 2) ? (operand_[4] >> 4) & 0x0F : 0;
	uint8_t vG = operand_[0] & 0x0F;

	switch (reg_count) {
		case 0:
			return fmt::format("{} method@{}", name_, method_idx);
		case 1:
			return fmt::format("{} {{v{}}}, method@{}", name_, vC, method_idx);
		case 2:
			return fmt::format("{} {{v{}, v{}}}, method@{}", name_, vC, vD, method_idx);
		case 3:
			return fmt::format("{} {{v{}, v{}, v{}}}, method@{}", name_, vC, vD, vE, method_idx);
		case 4:
			return fmt::format("{} {{v{}, v{}, v{}, v{}}}, method@{}", name_, vC, vD, vE, vF, method_idx);
		case 5:
			return fmt::format("{} {{v{}, v{}, v{}, v{}, v{}}}, method@{}", name_, vC, vD, vE, vF, vG, method_idx);
		default:
			return fmt::format("{} {{v{}, v{}, v{}, v{}, v{}}}, method@{} (unsupported reg count={})", name_, vC, vD, vE, vF, vG, method_idx, reg_count);
	}
}

std::string Disassembler::format_i3rc(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const {
	size_ += 5;
	uint16_t method_idx = *(const uint16_t*)&operand_[1];  // Method index
	uint16_t vCCCC = *(const uint16_t*)&operand_[3];       // Starting register
	uint8_t reg_count = operand_[0];                       // Number of registers

	uint16_t vNNNN = vCCCC + reg_count - 1;  // Ending register
	return fmt::format("{} {{v{} .. v{}}}, meth@{}", name_, vCCCC, vNNNN, method_idx);
}

std::string Disassembler::format_i51l(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const {
	size_ += 9;
	uint8_t vAA = operand_[0];
	int64_t imm = *(const int64_t*)&operand_[1] & 0x00FFFFFFFFFFFFFF;
	return fmt::format("{} v{}, #{}", name_, vAA, imm);
}
