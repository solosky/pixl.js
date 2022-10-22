/*
 * (c) 2021      nitz — chris marc dailey https://cmd.wtf
 * (c) 2017      Marcos Del Sol Vives
 *
 * SPDX-License-Identifier: MIT
 */

#include <nfc3d/version.h>
#include <stdio.h>

const char* nfc3d_version_fork()
{
	return "cmdwtf/Odiin";
}

uint32_t nfc3d_version_commit()
{
	return GIT_HASH_INT;
}

uint32_t nfc3d_version_build()
{
	return GIT_COMMIT_COUNT;
}
