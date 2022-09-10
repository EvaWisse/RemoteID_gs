/*
 * Copyright (c) 2012-2020 MIRACL UK Ltd.
 *
 * This file is part of MIRACL Core
 * (see https://github.com/miracl/core).
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CONFIG_CURVE_BN254_H
#define CONFIG_CURVE_BN254_H

#include"core.h"
#include"config_field_BN254.h"

// ECP stuff

#define CURVE_SECURITY_BN254 128

// Permit alternate compression method if 3 spare top bits in field representation
// Must be set manually
// #define ALLOW_ALT_COMPRESS_BN254

#if CURVE_SECURITY_BN254 == 128
#define HASH_TYPE_BN254 SHA256  /**< Hash type */
#endif

#if CURVE_SECURITY_BN254 == 192
#define HASH_TYPE_BN254 SHA384  /**< Hash type */
#endif

#if CURVE_SECURITY_BN254 == 256
#define HASH_TYPE_BN254 SHA512  /**< Hash type */
#endif

namespace BN254_BIG = B256_28;
namespace BN254_FP = BN254;

#endif
