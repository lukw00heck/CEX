#ifndef CEX_CRYPTOSYMMETRICCIPHEREXCEPTION_H
#define CEX_CRYPTOSYMMETRICCIPHEREXCEPTION_H

#include "CexDomain.h"

NAMESPACE_EXCEPTION

/// <summary>
/// Wraps exceptions thrown within a Symmetric cipher operational context
/// </summary>
struct CryptoSymmetricCipherException : std::exception
{
private:

	std::string m_details;
	std::string m_message;
	std::string m_origin;

public:

	//~~~Constructor~~~//

	/// <summary>
	/// Copy operator: copy is restricted, this function has been deleted
	/// </summary>
	CryptoSymmetricCipherException& operator=(const CryptoSymmetricCipherException&) = delete;

	/// <summary>
	/// Default constructor: default is restricted, this function has been deleted
	/// </summary>
	CryptoSymmetricCipherException() = delete;

	/// <summary>
	/// Constructor: instantiate this class with a message
	/// </summary>
	///
	/// <param name="Message">A custom message or error data</param>
	explicit CryptoSymmetricCipherException(const std::string &Message);

	/// <summary>
	/// Constructor: instantiate this class with an origin and message
	/// </summary>
	///
	/// <param name="Origin">The origin of the exception</param>
	/// <param name="Message">A custom message or error data</param>
	CryptoSymmetricCipherException(const std::string &Origin, const std::string &Message);

	/// <summary>
	/// Constructor: instantiate this class with an origin, message and inner exception
	/// </summary>
	///
	/// <param name="Origin">The origin of the exception</param>
	/// <param name="Message">A custom message or error data</param>
	/// <param name="Detail">The inner exception string</param>
	CryptoSymmetricCipherException(const std::string &Origin, const std::string &Message, const std::string &Detail);

	/// <summary>
	/// Destructor: finalize this class
	/// </summary>
	~CryptoSymmetricCipherException();

	//~~~Accessors~~~//

	/// <summary>
	/// Read/Write: The inner exception string
	/// </summary>
	std::string &Details();

	/// <summary>
	/// Read/Write: The message associated with the error
	/// </summary>
	std::string &Message();

	/// <summary>
	/// Read/Write: The origin of the exception in the format Class
	/// </summary>
	std::string &Origin();
};

NAMESPACE_EXCEPTIONEND
#endif
