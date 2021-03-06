#include "KDF2.h"
#include "DigestFromName.h"
#include "IntUtils.h"

NAMESPACE_KDF

const std::string KDF2::CLASS_NAME("KDF2");

//~~~Constructor~~~//

KDF2::KDF2(Digests DigestType)
	:
	m_msgDigest(DigestType != Digests::None ? Helper::DigestFromName::GetInstance(DigestType) :
		throw CryptoKdfException("KDF2:CTor", "Digest type can not be none!")),
	m_blockSize(m_msgDigest->BlockSize()),
	m_destroyEngine(true),
	m_hashSize(m_msgDigest->DigestSize()),
	m_isDestroyed(false),
	m_isInitialized(false),
	m_kdfCounter(1),
	m_kdfDigestType(DigestType),
	m_kdfKey(0),
	m_kdfSalt(0),
	m_legalKeySizes(0)
{
	LoadState();
}

KDF2::KDF2(Digest::IDigest* Digest)
	:
	m_msgDigest(Digest != nullptr ? Digest :
		throw CryptoKdfException("KDF2:CTor", "The Digest can not be null!")),
	m_blockSize(m_msgDigest->BlockSize()),
	m_destroyEngine(false),
	m_hashSize(m_msgDigest->DigestSize()),
	m_isDestroyed(false),
	m_isInitialized(false),
	m_kdfCounter(1),
	m_kdfDigestType(m_msgDigest->Enumeral()),
	m_kdfKey(0),
	m_kdfSalt(0),
	m_legalKeySizes(0)
{
	LoadState();
}

KDF2::~KDF2()
{
	if (!m_isDestroyed)
	{
		m_isDestroyed = true;
		m_blockSize = 0;
		m_kdfCounter = 0;
		m_kdfDigestType = Digests::None;
		m_hashSize = 0;
		m_isInitialized = false;

		Utility::IntUtils::ClearVector(m_kdfKey);
		Utility::IntUtils::ClearVector(m_kdfSalt);
		Utility::IntUtils::ClearVector(m_legalKeySizes);

		if (m_destroyEngine)
		{
			m_destroyEngine = false;

			if (m_msgDigest != nullptr)
			{
				m_msgDigest.reset(nullptr);
			}
		}
		else
		{
			if (m_msgDigest != nullptr)
			{
				m_msgDigest.release();
			}
		}
	}
}

//~~~Accessors~~~//

const Kdfs KDF2::Enumeral()
{ 
	return Kdfs::KDF2; 
}

const bool KDF2::IsInitialized() 
{ 
	return m_isInitialized; 
}

size_t KDF2::MinKeySize() 
{ 
	return m_blockSize;
}

std::vector<SymmetricKeySize> KDF2::LegalKeySizes() const 
{
	return m_legalKeySizes; 
}

const std::string KDF2::Name() 
{ 
	return CLASS_NAME + "-" + m_msgDigest->Name();
}

//~~~Public Functions~~~//

size_t KDF2::Generate(std::vector<byte> &Output)
{
	CexAssert(m_isInitialized, "the generator must be initialized before use");
	CexAssert(Output.size() != 0, "the output buffer too small");

	if (m_kdfCounter + (Output.size() / m_hashSize) > 255)
	{
		throw CryptoKdfException("KDF2:Generate", "KDF2 may only be used for 255 * HashLen bytes of output");
	}

	return Expand(Output, 0, Output.size());
}

size_t KDF2::Generate(std::vector<byte> &Output, size_t OutOffset, size_t Length)
{
	CexAssert(m_isInitialized, "the generator must be initialized before use");
	CexAssert(Output.size() != 0, "the output buffer too small");

	if (m_kdfCounter + (Length / m_hashSize) > 255)
	{
		throw CryptoKdfException("KDF2:Generate", "KDF2 may only be used for 255 * HashLen bytes of output");
	}

	return Expand(Output, OutOffset, Length);
}

void KDF2::Initialize(ISymmetricKey &GenParam)
{
	if (GenParam.Nonce().size() != 0)
	{
		if (GenParam.Info().size() != 0)
		{
			Initialize(GenParam.Key(), GenParam.Nonce(), GenParam.Info());
		}
		else
		{
			Initialize(GenParam.Key(), GenParam.Nonce());
		}
	}
	else
	{
		Initialize(GenParam.Key());
	}
}

void KDF2::Initialize(const std::vector<byte> &Key)
{
	if (Key.size() < m_hashSize)
	{
		throw CryptoKdfException("KDF2:Initialize", "Salt size is too small; must be a minumum of digest return size!");
	}

	if (m_isInitialized)
	{
		Reset();
	}

	// equal or less than a full block, interpret as ISO18033
	if (Key.size() <= m_blockSize)
	{
		// pad the key to one block
		m_kdfKey.resize(m_blockSize);
		Utility::MemUtils::Copy(Key, 0, m_kdfKey, 0, Key.size());
	}
	else
	{
		m_kdfKey.resize(m_blockSize);
		Utility::MemUtils::Copy(Key, 0, m_kdfKey, 0, m_blockSize);
		m_kdfSalt.resize(Key.size() - m_blockSize);
		Utility::MemUtils::Copy(Key, m_blockSize, m_kdfSalt, 0, m_kdfSalt.size());
	}

	m_isInitialized = true;
}

void KDF2::Initialize(const std::vector<byte> &Key, const std::vector<byte> &Salt)
{
	if (Key.size() < m_hashSize)
	{
		throw CryptoKdfException("KDF2:Initialize", "Key size is too small; must be a minumum of digest return size!");
	}
	if (Salt.size() < MIN_SALTLEN)
	{
		throw CryptoKdfException("KDF2:Initialize", "Salt size is too small; must be a minumum of 4 bytes!");
	}

	if (m_isInitialized)
	{
		Reset();
	}

	m_kdfKey.resize(Key.size());
	Utility::MemUtils::Copy(Key, 0, m_kdfKey, 0, Key.size());

	if (Salt.size() > 0)
	{
		m_kdfSalt.resize(Salt.size());
		Utility::MemUtils::Copy(Salt, 0, m_kdfSalt, 0, Salt.size());
	}

	m_isInitialized = true;
}

void KDF2::Initialize(const std::vector<byte> &Key, const std::vector<byte> &Salt, const std::vector<byte> &Info)
{
	if (Key.size() < m_hashSize)
	{
		throw CryptoKdfException("KDF2:Initialize", "Key size is too small; must be a minumum of digest return size!");
	}
	if (Salt.size() < MIN_SALTLEN)
	{
		throw CryptoKdfException("KDF2:Initialize", "Salt size is too small; must be a minumum of 4 bytes!");
	}

	if (m_isInitialized)
	{
		Reset();
	}

	m_kdfKey.resize(Key.size());
	Utility::MemUtils::Copy(Key, 0, m_kdfKey, 0, Key.size());

	if (Salt.size() > 0)
	{
		m_kdfSalt.resize(Salt.size() + Info.size());
		Utility::MemUtils::Copy(Salt, 0, m_kdfSalt, 0, Salt.size());

		// add info as extension of salt
		if (Info.size() > 0)
		{
			Utility::MemUtils::Copy(Info, 0, m_kdfSalt, Salt.size(), Info.size());
		}
	}

	m_isInitialized = true;
}

void KDF2::ReSeed(const std::vector<byte> &Seed)
{
	if (Seed.size() < m_hashSize)
	{
		throw CryptoKdfException("KDF2:Update", "Seed is too small!");
	}

	Initialize(Seed);
}

void KDF2::Reset()
{
	m_msgDigest->Reset();
	m_kdfCounter = 1;
	m_kdfKey.clear();
	m_kdfSalt.clear();
	m_isInitialized = false;
}

//~~~Private Functions~~~//

size_t KDF2::Expand(std::vector<byte> &Output, size_t OutOffset, size_t Length)
{
	if (m_kdfCounter + (Length / m_hashSize) > 255)
	{
		throw CryptoKdfException("KDF2:Expand", "Maximum length value is 255 * the digest return size!");
	}

	std::vector<byte> hash(m_hashSize);
	size_t prcLen = Length;

	do
	{
		m_msgDigest->Update(m_kdfKey, 0, m_kdfKey.size());
		m_msgDigest->Update(static_cast<byte>(m_kdfCounter >> 24));
		m_msgDigest->Update(static_cast<byte>(m_kdfCounter >> 16));
		m_msgDigest->Update(static_cast<byte>(m_kdfCounter >> 8));
		m_msgDigest->Update(static_cast<byte>(m_kdfCounter));

		if (m_kdfSalt.size() != 0)
		{
			m_msgDigest->Update(m_kdfSalt, 0, m_kdfSalt.size());
		}

		m_msgDigest->Finalize(hash, 0);
		++m_kdfCounter;

		size_t prcRmd = Utility::IntUtils::Min(m_hashSize, prcLen);
		Utility::MemUtils::Copy(hash, 0, Output, OutOffset, prcRmd);
		prcLen -= prcRmd;
		OutOffset += prcRmd;
	}
	while (prcLen != 0);

	return Length;
}

void KDF2::LoadState()
{
	// best salt size; hash finalizer code and counter length adjusted
	size_t saltLen = m_blockSize - (Helper::DigestFromName::GetPaddingSize(m_kdfDigestType) + sizeof(uint));
	m_legalKeySizes.resize(3);
	// minimum security is the digest output size
	m_legalKeySizes[0] = SymmetricKeySize(m_hashSize, 0, 0);
	// recommended size, adjusted salt size to hash full blocks
	m_legalKeySizes[1] = SymmetricKeySize(m_blockSize, saltLen, 0);
	// max recommended; add a block of key to info (appended to salt)
	m_legalKeySizes[2] = SymmetricKeySize(m_blockSize, saltLen, m_blockSize);
}

NAMESPACE_KDFEND
