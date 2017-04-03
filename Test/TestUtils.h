#ifndef _CEXTEST_TESTUTILS_H
#define _CEXTEST_TESTUTILS_H

#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif

#include <fstream>
#include <iostream>
#include <algorithm>
#include "../CEX/CexDomain.h"
#include "../CEX/CSP.h"
#include "../CEX/SymmetricKey.h"
#include "TestException.h"

namespace Test
{
	using namespace CEX;

	class TestUtils
	{
	public:

		/// <summary>
		/// Copy an int vector array
		/// </summary>
		static void CopyVector(const std::vector<int> &SrcArray, size_t SrcIndex, std::vector<int> &DstArray, size_t DstIndex, size_t Length)
		{
			memcpy(&DstArray[DstIndex], &SrcArray[SrcIndex], Length * sizeof(SrcArray[SrcIndex]));
		}

		/// <summary>
		/// Compares two byte arrays
		/// </summary>
		/// 
		/// <returns>Arrays Are Equal</returns>
		static bool IsEqual(std::vector<byte> &A, std::vector<byte> &B)
		{
			size_t i = A.size();

			if (i != B.size())
				return false;

			while (i != 0)
			{
				--i;
				if (A[i] != B[i])
					return false;
			}

			return true;
		}

		/// <summary>
		/// Returns the amount of milliseconds elapsed since the UNIX epoch. Works on both
		/// windows and linux. See http://stackoverflow.com/questions/1861294/how-to-calculate-execution-time-of-a-code-snippet-in-c
		/// and http://stackoverflow.com/questions/1604582/timing-program-runtimes-in-visual-c
		/// </summary>
		/// 
		/// <returns>Time in Milliseconds</returns>
		static uint64_t GetTimeMs64()
		{
#ifdef _WIN32
			// Windows
			__int64 ctr1 = 0, freq = 0;

			if (QueryPerformanceCounter((LARGE_INTEGER *)&ctr1) != 0)
			{
				QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
				// return microseconds to milliseconds
				return (uint64_t)(ctr1 * 1000.0 / freq);
			}
			else
			{
				FILETIME ft;
				LARGE_INTEGER li;

				// Get the amount of 100 nano seconds intervals elapsed since January 1, 1601 (UTC) and copy it to a LARGE_INTEGER structure
				GetSystemTimeAsFileTime(&ft);
				li.LowPart = ft.dwLowDateTime;
				li.HighPart = ft.dwHighDateTime;

				uint64_t ret = li.QuadPart;
				ret -= 116444736000000000LL; // Convert from file time to UNIX epoch time.
				ret /= 10000; // From 100 nano seconds (10^-7) to 1 millisecond (10^-3) intervals

				return ret;
			}
#else
			// Linux
			struct timeval tv;

			gettimeofday(&tv, NULL);
			uint64_t ret = tv.tv_usec;
			// Convert from micro seconds (10^-6) to milliseconds (10^-3)
			ret /= 1000;
			// Adds the seconds (10^0) after converting them to milliseconds (10^-3)
			ret += (tv.tv_sec * 1000);

			return ret;
#endif
		}

		/// <summary>
		/// Initializes a SymmetricKey structure with pseudo random data
		/// </summary>
		static Key::Symmetric::SymmetricKey GetRandomKey(size_t KeySize, size_t IvSize)
		{
			Provider::CSP rng;
			std::vector<byte> key(KeySize, 0);
			std::vector<byte> iv(IvSize, 0);
			rng.GetBytes(key);
			rng.GetBytes(iv);

			return Key::Symmetric::SymmetricKey(key, iv);
		}

		/// <summary>
		/// Fill a byte vector with pseudo random data
		/// </summary>
		static void GetRandom(std::vector<byte> &Data)
		{
			Provider::CSP rng;
			rng.GetBytes(Data);
		}

		/// <summary>
		/// Reads a file into a buffer
		/// </summary>
		/// 
		/// <returns>Success</returns>
		static bool Read(const std::string &FilePath, std::string &Contents)
		{
			bool status = false;
			std::ifstream ifs(FilePath, std::ios::binary | std::ios::ate);

			if (!ifs || !ifs.is_open())
			{
				throw TestException("Could not open the KAT file!");
			}
			else
			{
				ifs.seekg(0, std::ios::end);
				const int bufsize = (int)ifs.tellg();
				ifs.seekg(0, std::ios::beg);

				if (bufsize > 0)
				{
					status = true;
					std::vector<char> bufv(bufsize, 0);
					char *buf = &bufv[0];
					ifs.read(buf, bufsize);
					Contents.assign(buf, bufsize);
				}
				else
				{
					throw TestException("The KAT file is empty!");
				}
			}

			return status;
		}

		static std::vector<byte> Reduce(std::vector<byte> Seed)
		{
			unsigned int len = (unsigned int)(Seed.size() / 2);
			std::vector<byte> data(len);

			for (unsigned int i = 0; i < len; i++)
				data[i] = (byte)(Seed[i] ^ Seed[len + i]);

			return data;
		}

		/// <summary>
		/// Reverses a byte array
		/// </summary>
		static void Reverse(std::vector<byte> &Data)
		{
			std::reverse(Data.begin(), Data.end());
		}
	};
}
#endif
