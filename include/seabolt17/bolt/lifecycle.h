/*
* Copyright (c) 2002-2018 "Neo4j,"
* Neo4j Sweden AB [http://neo4j.com]
*
* This file is part of Neo4j.
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

#ifndef SEABOLT_BOLT
#define SEABOLT_BOLT

#include "bolt-public.h"

/**
 * Carries out global initialization of the external dependencies like OpenSSL, Winsocks, etc.
 *
 * Must be called before any other connector functions are invoked.
 */
SEABOLT_EXPORT void Bolt_startup();

/**
 * Carries out global cleanup as required by external dependencies.
 *
 * Must be called to perform a clean shutdown. No more interactions with the API should occur
 * after this call.
 */
SEABOLT_EXPORT void Bolt_shutdown();

#endif // SEABOLT_BOLT
