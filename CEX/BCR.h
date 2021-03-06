// The GPL version 3 License (GPLv3)
// 
// Copyright (c) 2017 vtdev.com
// This file is part of the CEX Cryptographic library.
// 
// This program is free software : you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
// 
// Implementation Details:
// An implementation of a Block cipher Counter based Generator (BCR). 
// Written by John Underhill, January 6, 2014
// Contact: develop@vtdev.com

#ifndef CEX_BCR_H
#define CEX_BCR_H

#include "BlockCiphers.h"
#include "BCG.h"
#include "IPrng.h"
#include "Providers.h"

NAMESPACE_PRNG

using Enumeration::BlockCiphers;
using Enumeration::Providers;

/// <summary>
/// An implementation of a Block cipher Counter mode PRNG.
/// <para>Note* as of version 1.0.0.2, the order of the Minimum and Maximum parameters on the NextIntXX api has changed, it is now with the Maximum parameter first, ex. NextInt16(max, min).</para>
/// </summary> 
/// 
/// <example>
/// <description>Example of generating a pseudo random integer:</description>
/// <code>
/// BCR rnd([BlockCiphers], [Providers]);
/// // get random int
/// int num = rnd.NextUInt32([Minimum], [Maximum]);
/// </code>
/// </example>
/// 
/// <remarks>
/// <description>Implementation Notes:</description>
/// <list type="bullet">
/// <item><description>Wraps the Counter Mode Generator (BCG) drbg implementation.</description></item>
/// <item><description>Can be initialized with any of the implemented block ciphers.</description></item>
/// <item><description>Can use either a random seed generator for initialization, or a user supplied Seed array.</description></item>
/// <item><description>Using the same seed value will produce the same random output.</description></item>
/// </list>
/// 
/// <description>Guiding Publications:</description>
/// <list type="number">
/// <item><description>NIST <a href="http://csrc.nist.gov/publications/drafts/800-90/draft-sp800-90b.pdf">SP800-90B</a>: Recommendation for the Entropy Sources Used for Random Bit Generation.</description></item>
/// <item><description>NIST <a href="http://csrc.nist.gov/publications/fips/fips140-2/fips1402.pdf">Fips 140-2</a>: Security Requirments For Cryptographic Modules.</description></item>
/// <item><description>NIST <a href="http://csrc.nist.gov/groups/ST/toolkit/rng/documents/SP800-22rev1a.pdf">SP800-22 1a</a>: A Statistical Test Suite for Random and Pseudorandom Number Generators for Cryptographic Applications.</description></item>
/// <item><description>NIST <a href="http://eprint.iacr.org/2006/379.pdf">Security Bounds</a> for the Codebook-based: Deterministic Random Bit Generator.</description></item>
/// </list>
/// </remarks>
class BCR final : public IPrng
{
private:

	static const size_t BLOCK_SIZE = 16;
	static const size_t BUFFER_DEF = 4096;
	static const size_t BUFFER_MIN = 64;
	static const std::string CLASS_NAME;

	size_t m_bufferIndex;
	BlockCiphers m_engineType;
	bool m_isDestroyed;
	bool m_isParallel;
	Providers m_pvdType;
	std::vector<byte>  m_rndSeed;
	std::vector<byte> m_rngBuffer;
	std::unique_ptr<Drbg::BCG> m_rngGenerator;

public:

	//~~~Constructor~~~//

	/// <summary>
	/// Copy constructor: copy is restricted, this function has been deleted
	/// </summary>
	BCR(const BCR&) = delete;

	/// <summary>
	/// Copy operator: copy is restricted, this function has been deleted
	/// </summary>
	BCR& operator=(const BCR&) = delete;

	/// <summary>
	/// Initialize this class with parameters
	/// </summary>
	/// 
	/// <param name="CipherType">The block cipher that powers the rng (default is AHX)</param>
	/// <param name="ProviderType">The Seed engine used to create keyng material (default is none)</param>
	/// <param name="Parallel">Run the underlying CTR mode generator in parallel mode</param>
	/// 
	/// <exception cref="Exception::CryptoRandomException">Thrown if the selected parameters are invalid</exception>
	explicit BCR(BlockCiphers CipherType = BlockCiphers::AHX, Providers ProviderType = Providers::None, bool Parallel = true);

	/// <summary>
	/// Initialize the class with a Seed; note: the same seed will produce the same random output
	/// </summary>
	/// 
	/// <param name="Seed">The Seed bytes used to initialize the digest counter; (min. length is key size + counter 16)</param>
	/// <param name="CipherType">The block cipher that powers the rng (default is AHX)</param>
	/// <param name="Parallel">Run the underlying CTR mode generator in parallel mode</param>
	/// 
	/// <exception cref="Exception::CryptoRandomException">Thrown if the selected parameters are invalid</exception>
	explicit BCR(std::vector<byte> &Seed, BlockCiphers CipherType = BlockCiphers::AHX, bool Parallel = true);

	/// <summary>
	/// Destructor: finalize this class
	/// </summary>
	~BCR() override;

	//~~~Accessors~~~//

	/// <summary>
	/// Read Only: The random generators type name
	/// </summary>
	const Prngs Enumeral() override;

	/// <summary>
	/// Read Only: The random generators class name
	/// </summary>
	const std::string Name() override;

	//~~~Public Functions~~~//

	/// <summary>
	/// Fill an array of uint16 with pseudo-random
	/// </summary>
	///
	/// <param name="Output">The uint16 output array</param>
	/// <param name="Offset">The starting index within the Output array</param>
	/// <param name="Elements">The number of array elements to fill</param>
	void Fill(std::vector<ushort> &Output, size_t Offset, size_t Elements) override;

	/// <summary>
	/// Fill an array of uint32 with pseudo-random
	/// </summary>
	///
	/// <param name="Output">The uint32 output array</param>
	/// <param name="Offset">The starting index within the Output array</param>
	/// <param name="Elements">The number of array elements to fill</param>
	void Fill(std::vector<uint> &Output, size_t Offset, size_t Elements) override;

	/// <summary>
	/// Fill an array of uint64 with pseudo-random
	/// </summary>
	///
	/// <param name="Output">The uint64 output array</param>
	/// <param name="Offset">The starting index within the Output array</param>
	/// <param name="Elements">The number of array elements to fill</param>
	void Fill(std::vector<ulong> &Output, size_t Offset, size_t Elements) override;

	/// <summary>
	/// Return an array filled with pseudo random bytes
	/// </summary>
	/// 
	/// <param name="Length">Size of requested byte array</param>
	/// 
	/// <returns>Random byte array</returns>
	std::vector<byte> GetBytes(size_t Length) override;

	/// <summary>
	/// Fill the buffer with pseudo-random bytes using offsets
	/// </summary>
	///
	/// <param name="Output">The output array to fill</param>
	/// <param name="Offset">The starting position within the Output array</param>
	/// <param name="Length">The number of bytes to write to the Output array</param>
	void GetBytes(std::vector<byte> &Output, size_t Offset, size_t Length) override;

	/// <summary>
	/// Fill an array with pseudo random bytes
	/// </summary>
	///
	/// <param name="Output">Output array</param>
	void GetBytes(std::vector<byte> &Output) override;

	/// <summary>
	/// Get a pseudo random unsigned 16bit integer
	/// </summary>
	/// 
	/// <returns>Random UInt16</returns>
	ushort NextUInt16() override;

	/// <summary>
	/// Get a pseudo random unsigned 32bit integer
	/// </summary>
	/// 
	/// <returns>Random 32bit integer</returns>
	uint NextUInt32() override;

	/// <summary>
	/// Get a pseudo random unsigned 64bit integer
	/// </summary>
	/// 
	/// <returns>Random 64bit integer</returns>
	ulong NextUInt64() override;

	/// <summary>
	/// Reset the generator instance
	/// </summary>
	void Reset() override;
};

NAMESPACE_PRNGEND
#endif
