﻿// The GPL version 3 License (GPLv3)
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

#ifndef CEX_IBLOCKCIPHER_H
#define CEX_IBLOCKCIPHER_H

#include "CexDomain.h"
#include "BlockCiphers.h"
#include "CryptoSymmetricCipherException.h"
#include "IDigest.h"
#include "ISymmetricKey.h"
#include "SymmetricKeySize.h"

NAMESPACE_BLOCK

using Enumeration::BlockCiphers;
using Exception::CryptoSymmetricCipherException;
using Enumeration::Digests;
using Digest::IDigest;
using Key::Symmetric::ISymmetricKey;
using Key::Symmetric::SymmetricKeySize;

/// <summary>
/// The Block Cipher Interface class
/// </summary> 
class IBlockCipher
{
public:

	//~~~Constructor~~~//

	/// <summary>
	/// Copy constructor: copy is restricted, this function has been deleted
	/// </summary>
	IBlockCipher(const IBlockCipher&) = delete;

	/// <summary>
	/// Copy operator: copy is restricted, this function has been deleted
	/// </summary>
	IBlockCipher& operator=(const IBlockCipher&) = delete;

	/// <summary>
	/// Constructor: Instantiate this class
	/// </summary>
	IBlockCipher() 
	{
	}

	/// <summary>
	/// Destructor: finalize this class
	/// </summary>
	virtual ~IBlockCipher() noexcept 
	{
	}

	//~~~Accessors~~~//

	/// <summary>
	/// Read Only: Unit block size of internal cipher in bytes
	/// </summary>
	virtual const size_t BlockSize() = 0;

	/// <summary>
	/// Read/Write: Reads or Sets the Info (personalization string) value in the HKDF initialization parameters.
	/// <para>Changing this code will create a unique distribution of the cipher.
	/// Code can be sized as either a zero byte array, or any length up to the DistributionCodeMax size.
	/// For best security, the distribution code should be random, secret, and equal in length to the DistributionCodeMax size.
	/// Note: If the Info parameter of an ISymmetricKey is non-zero, it will overwrite the distribution code.</para>
	/// </summary>
	virtual std::vector<byte> &DistributionCode() = 0;

	/// <summary>
	/// Read Only: The maximum size of the distribution code in bytes.
	/// <para>The distribution code can be used as a secondary source of entropy (secret) in the HKDF key expansion phase.
	/// If used as a nonce the distribution code should be secret, and equal in size to this value</para>
	/// </summary>
	virtual const size_t DistributionCodeMax() = 0;

	/// <summary>
	/// Read Only: The block ciphers type name
	/// </summary>
	virtual const BlockCiphers Enumeral() = 0;

	/// <summary>
	/// Read Only: True is initialized for encryption, false for decryption.
	/// <para>Value set in <see cref="Initialize(bool, ISymmetricKey)"/>.</para>
	/// </summary>
	virtual const bool IsEncryption() = 0;

	/// <summary>
	/// Read Only: Cipher is ready to transform data
	/// </summary>
	virtual const bool IsInitialized() = 0;

	/// <summary>
	/// Read Only: The extended ciphers HKDF digest type
	/// </summary>
	virtual const Digests KdfEngine() = 0;

	/// <summary>
	/// Read Only: Available Encryption Key Sizes in bytes
	/// </summary>
	virtual const std::vector<SymmetricKeySize> &LegalKeySizes() = 0;

	/// <summary>
	/// Read Only: Available transformation round assignments
	/// </summary>
	virtual const std::vector<size_t> &LegalRounds() = 0;

	/// <summary>
	/// Read Only: The block ciphers class name
	/// </summary>
	virtual const std::string Name() = 0;

	/// <summary>
	/// Read Only: The number of transformation rounds processed by the transform
	/// </summary>
	virtual const size_t Rounds() = 0;

	/// <summary>
	/// Read Only: The sum size in bytes (plus some allowance for externals) of the classes persistant state.
	/// <para>Used in parallel block calculation to reduce L1 cache eviction occurence. see ParallelOptions</para>
	/// </summary>
	virtual const size_t StateCacheSize() = 0;

	//~~~Public Functions~~~//

	/// <summary>
	/// Decrypt a single block of bytes.
	/// <para><see cref="Initialize(bool, ISymmetricKey)"/> must be called with the Encryption flag set to <c>false</c> before this method can be used.
	/// Input and Output arrays must be at least <see cref="BlockSize"/> in length.</para>
	/// </summary>
	///
	/// <param name="Input">Encrypted bytes</param>
	/// <param name="Output">Decrypted bytes</param>
	virtual void DecryptBlock(const std::vector<byte> &Input, std::vector<byte> &Output) = 0;

	/// <summary>
	/// Decrypt a block of bytes with offset parameters.
	/// <para><see cref="Initialize(bool, ISymmetricKey)"/> must be called with the Encryption flag set to <c>false</c> before this method can be used.
	/// Input and Output arrays with Offsets must be at least <see cref="BlockSize"/> in length.</para>
	/// </summary>
	/// 
	/// <param name="Input">Encrypted bytes</param>
	/// <param name="InOffset">Starting offset within the input array</param>
	/// <param name="Output">Decrypted bytes</param>
	/// <param name="OutOffset">Starting offset within the output array</param>
	virtual void DecryptBlock(const std::vector<byte> &Input, const size_t InOffset, std::vector<byte> &Output, const size_t OutOffset) = 0;

	/// <summary>
	/// Encrypt a block of bytes.
	/// <para><see cref="Initialize(bool, ISymmetricKey)"/> must be called with the Encryption flag set to <c>true</c> before this method can be used.
	/// Input and Output array lengths must be at least <see cref="BlockSize"/> in length.</para>
	/// </summary>
	/// 
	/// <param name="Input">The input array of bytes to transform</param>
	/// <param name="Output">The output array of transformed bytes</param>
	virtual void EncryptBlock(const std::vector<byte> &Input, std::vector<byte> &Output) = 0;

	/// <summary>
	/// Encrypt a block of bytes with offset parameters.
	/// <para><see cref="Initialize(bool, ISymmetricKey)"/> must be called with the Encryption flag set to <c>true</c> before this method can be used.
	/// Input and Output arrays with Offsets must be at least <see cref="BlockSize"/> in length.</para>
	/// </summary>
	/// 
	/// <param name="Input">The input array of bytes to transform</param>
	/// <param name="InOffset">Starting offset within the input array</param>
	/// <param name="Output">The output array of transformed bytes</param>
	/// <param name="OutOffset">Starting offset within the output array</param>
	virtual void EncryptBlock(const std::vector<byte> &Input, const size_t InOffset, std::vector<byte> &Output, const size_t OutOffset) = 0;

	/// <summary>
	/// Initialize the cipher
	/// </summary>
	/// 
	/// <param name="Encryption">Using Encryption or Decryption mode</param>
	/// <param name="KeyParams">Cipher key container. 
	/// <para>The <see cref="LegalKeySizes"/> property contains valid sizes.</para></param>
	/// 
	/// <exception cref="Exception::CryptoSymmetricCipherException">Thrown if a null or invalid key is used</exception>
	virtual void Initialize(bool Encryption, ISymmetricKey &KeyParams) = 0;

	/// <summary>
	/// Transform a block of bytes.
	/// <para><see cref="Initialize(bool, ISymmetricKey)"/> must be called before this method can be used.
	/// Input and Output array lengths must be at least <see cref="BlockSize"/> in length.</para>
	/// </summary>
	/// 
	/// <param name="Input">The input array of bytes to transform</param>
	/// <param name="Output">The output array of transformed bytes</param>
	virtual void Transform(const std::vector<byte> &Input, std::vector<byte> &Output) = 0;

	/// <summary>
	/// Transform a block of bytes with offset parameters.
	/// <para><see cref="Initialize(bool, ISymmetricKey)"/> must be called before this method can be used.
	/// Input and Output arrays with Offsets must be at least <see cref="BlockSize"/> in length.</para>
	/// </summary>
	/// 
	/// <param name="Input">The input array of bytes to transform</param>
	/// <param name="InOffset">Starting offset within the input array</param>
	/// <param name="Output">The output array of transformed bytes</param>
	/// <param name="OutOffset">Starting offset within the output array</param>
	virtual void Transform(const std::vector<byte> &Input, const size_t InOffset, std::vector<byte> &Output, const size_t OutOffset) = 0;

	/// <summary>
	/// Transform 4 blocks of bytes.
	/// <para><see cref="Initialize(bool, ISymmetricKey)"/> must be called before this method can be used.
	/// Input and Output array lengths must be at least 4 * <see cref="BlockSize"/> in length.</para>
	/// </summary>
	/// 
	/// <param name="Input">The input array of bytes to transform</param>
	/// <param name="InOffset">Starting offset in the Input array</param>
	/// <param name="Output">The output array of transformed bytes</param>
	/// <param name="OutOffset">Starting offset in the output array</param>
	virtual void Transform512(const std::vector<byte> &Input, const size_t InOffset, std::vector<byte> &Output, const size_t OutOffset) = 0;

	/// <summary>
	/// Transform 8 blocks of bytes.
	/// <para><see cref="Initialize(bool, ISymmetricKey)"/> must be called before this method can be used.
	/// Input and Output array lengths must be at least 8 * <see cref="BlockSize"/> in length.</para>
	/// </summary>
	/// 
	/// <param name="Input">The input array of bytes to transform</param>
	/// <param name="InOffset">Starting offset in the Input array</param>
	/// <param name="Output">The output array of transformed bytes</param>
	/// <param name="OutOffset">Starting offset in the output array</param>
	virtual void Transform1024(const std::vector<byte> &Input, const size_t InOffset, std::vector<byte> &Output, const size_t OutOffset) = 0;

	/// <summary>
	/// Transform 16 blocks of bytes.
	/// <para><see cref="Initialize(bool, ISymmetricKey)"/> must be called before this method can be used.
	/// Input and Output array lengths must be at least 16 * <see cref="BlockSize"/> in length.</para>
	/// </summary>
	/// 
	/// <param name="Input">The input array of bytes to transform</param>
	/// <param name="InOffset">Starting offset in the Input array</param>
	/// <param name="Output">The output array of transformed bytes</param>
	/// <param name="OutOffset">Starting offset in the output array</param>
	virtual void Transform2048(const std::vector<byte> &Input, const size_t InOffset, std::vector<byte> &Output, const size_t OutOffset) = 0;
};

NAMESPACE_BLOCKEND
#endif

