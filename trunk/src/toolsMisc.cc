/***************************************************************************
 *            toolsMisc.cc
 *
 *  Wed Aug 24 00:43:21 2005
 *  Copyright  2005  Ni@m
 *  niam.niam@gmail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License version 2.1 as published by
 *  the Free Software Foundation;
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/toolsMisc.h>

using namespace dodo::tools;

inline dodoString
misc::dummyTools(const dodoString &data)
{
	return data;
}

//-------------------------------------------------------------------

void
misc::random(void *data,
			 unsigned long size,
			 short strength)
{
	FILE *file;

	if (strength == MISC_RANDOMSTRENGTH_DEFAULT)
	{
		file = fopen("/dev/urandom", "r");
		if (file == NULL)
			throw baseEx(ERRMODULE_TOOLSMISC, MISCEX_RANDOM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
	else
	{
		if (strength == MISC_RANDOMSTRENGTH_STRONG)
		{
			file = fopen("/dev/random", "r");
			if (file == NULL)
				throw baseEx(ERRMODULE_TOOLSMISC, MISCEX_RANDOM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}
		else
			throw baseEx(ERRMODULE_TOOLSMISC, MISCEX_RANDOM, ERR_LIBDODO, MISCEX_WRONGSTRENGTH, TOOLSMISCEX_WRONGSTRENGTH_STR, __LINE__, __FILE__);
	}

	while (true)
	{
		if (fread(data, size, 1, file) == 0)
		{
			if (feof(file) != 0 || errno == EAGAIN)
				break;

			if (errno == EINTR)
				continue;

			if (ferror(file) != 0)
				throw baseEx(ERRMODULE_TOOLSMISC, MISCEX_RANDOM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		break;
	}

	if (fclose(file) != 0)
		throw baseEx(ERRMODULE_TOOLSMISC, MISCEX_RANDOM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

dodoString
misc::stringRandom(unsigned long size,
				   short strength)
{
	char *data = new char[size + 1];
	try
	{
		random(data, size, strength);
	}
	catch (...)
	{
		delete [] data;

		throw;
	}
	for (unsigned long i = 0; i < size; ++i)
		if (data[i] == '\0')
			data[i] = '*';

	dodoString res(data, size);
	delete [] data;

	return res;
}

//-------------------------------------------------------------------

unsigned long
misc::ulRandom(short strength)
{
	unsigned long rnd;

	random(&rnd, sizeof(unsigned long), strength);

	return rnd;
}

//-------------------------------------------------------------------

long
misc::lRandom(short strength)
{
	long rnd;

	random(&rnd, sizeof(long), strength);

	return rnd;
}

//-------------------------------------------------------------------

unsigned int
misc::uiRandom(short strength)
{
	unsigned int rnd;

	random(&rnd, sizeof(unsigned int), strength);

	return rnd;
}

//-------------------------------------------------------------------

int
misc::iRandom(short strength)
{
	int rnd;

	random(&rnd, sizeof(int), strength);

	return rnd;
}

//-------------------------------------------------------------------

unsigned short
misc::usRandom(short strength)
{
	unsigned short rnd;

	random(&rnd, sizeof(unsigned short), strength);

	return rnd;
}

//-------------------------------------------------------------------

short
misc::sRandom(short strength)
{
	short rnd;

	random(&rnd, sizeof(short), strength);

	return rnd;
}

//-------------------------------------------------------------------

unsigned char
misc::ucRandom(short strength)
{
	unsigned char rnd;

	random(&rnd, sizeof(unsigned char), strength);

	return rnd;
}

//-------------------------------------------------------------------

char
misc::cRandom(short strength)
{
	char rnd;

	random(&rnd, sizeof(char), strength);

	return rnd;
}

//-------------------------------------------------------------------

double
misc::dRandom(short strength)
{
	double rnd;

	random(&rnd, sizeof(double), strength);

	return rnd;
}

//-------------------------------------------------------------------

bool
misc::isInArray(const dodoStringArray &arr,
				const dodoString &needle,
				bool icase)
{
	bool (*cmpFunc)(const dodoString &, const dodoString &);

	if (icase)
		cmpFunc = string::iequal;
	else
		cmpFunc = string::equal;

	dodoStringArray::const_iterator i(arr.begin()), j(arr.end());
	for (; i != j; ++i)
		if (cmpFunc(*i, needle))
			return true;

	return false;
}

//-------------------------------------------------------------------

bool
misc::isInList(const dodoStringList &arr,
				const dodoString &needle,
				bool icase)
{
	bool (*cmpFunc)(const dodoString &, const dodoString &);

	if (icase)
		cmpFunc = string::iequal;
	else
		cmpFunc = string::equal;

	dodoStringList::const_iterator i(arr.begin()), j(arr.end());
	for (; i != j; ++i)
		if (cmpFunc(*i, needle))
			return true;

	return false;
}

//-------------------------------------------------------------------

dodo::dodoStringArray
misc::explode(const dodoString &fields,
			  escape escapeF,
			  const dodoString &separator,
			  int limit)
{
	unsigned long i(0), j(0), sep_size(separator.size());
	int k(1);
	dodoStringArray arr;

	while (true)
	{
		if (limit != -1)
		{
			if (k > limit)
			{
				arr.back().append(escapeF(fields.substr(j - sep_size)));

				break;
			}
			++k;
		}

		i = fields.find(separator, i);
		arr.push_back(escapeF(fields.substr(j, i - j)));
		if (i == dodoString::npos)
			break;

		i += sep_size;
		j = i;
	}

	return arr;
}

//-------------------------------------------------------------------

dodo::dodoStringArray
misc::explode(const dodoString &fields,
			  const dodoString &separator,
			  int limit)
{
	return explode(fields, &dummyTools, separator, limit);
}

//-------------------------------------------------------------------

dodoString
misc::implode(const dodoStringArray &fields,
			  escape escapeF,
			  const dodoString &separator,
			  const dodoString &frame,
			  int limit)
{
	if (fields.size() == 0)
		throw baseEx(ERRMODULE_TOOLSMISC, MISCEX_IMPLODE, ERR_LIBDODO, MISCEX_EMPTYARRAY, TOOLSMISCEX_EMPTYARRAY_STR, __LINE__, __FILE__);

	int k(0);

	dodoString temp, fs(frame + separator);
	dodoStringArray::const_iterator i(fields.begin()), j(fields.end());
	if (i != j)
	{
		--j;
		for (; i != j; ++i)
		{
			if (limit != -1)
			{
				if (k > limit)
					return temp;
				++k;
			}
			temp.append(frame + escapeF(*i) + fs);
		}
		temp.append(frame + escapeF(*i) + frame);
	}

	return temp;
}

//-------------------------------------------------------------------

dodoString
misc::implode(const dodoStringArray &fields,
			  const dodoString &separator,
			  const dodoString &frame,
			  int limit)
{
	return implode(fields, &dummyTools, separator, frame, limit);
}

//-------------------------------------------------------------------

dodoString
misc::implode(const dodoStringArray &fields,
			  const dodoString &separator,
			  int limit)
{
	return implode(fields, &dummyTools, separator, limit);
}

//-------------------------------------------------------------------

dodoString
misc::implode(const dodoStringArray &fields,
			  escape escapeF,
			  const dodoString &separator,
			  int limit)
{
	if (fields.size() == 0)
		throw baseEx(ERRMODULE_TOOLSMISC, MISCEX_IMPLODE, ERR_LIBDODO, MISCEX_EMPTYARRAY, TOOLSMISCEX_EMPTYARRAY_STR, __LINE__, __FILE__);

	int k(0);

	dodoString temp;
	dodoStringArray::const_iterator i(fields.begin()), j(fields.end());
	if (i != j)
	{
		--j;
		for (; i != j; ++i)
		{
			if (limit != -1)
			{
				if (k > limit)
					return temp;
				++k;
			}
			temp.append(escapeF(*i) + separator);
		}
		temp.append(escapeF(*i));
	}

	return temp;
}

//-------------------------------------------------------------------

void
misc::mail(const dodoString &to,
		   const dodoString &subject,
		   const dodoString &message,
		   const dodoString &headers,
		   const dodoString &path)
{
	FILE *sendmail = popen((path + " " + to).c_str(), "w");

	if (sendmail == NULL)
		throw baseEx(ERRMODULE_TOOLSMISC, MISCEX_MAIL, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	fprintf(sendmail, "To: %s\n", to.c_str());
	fprintf(sendmail, "Subject: %s\n", subject.c_str());
	if (headers.size() > 0)
		fprintf(sendmail, "%s\n", headers.c_str());
	fprintf(sendmail, "\n%s\n", message.c_str());

	if (pclose(sendmail) == -1)
		throw baseEx(ERRMODULE_TOOLSMISC, MISCEX_MAIL, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
misc::mail(const dodoString &host,
		   int port,
		   const dodoString &to,
		   const dodoString &from,
		   const dodoString &subject,
		   const dodoString &message,
		   const dodoString &login,
		   const dodoString &pass,
		   const dodoString &headers)
{
	using namespace io::network;

	enum mailAuthTypeEnum
	{
		SMTPAUTH_CRAMMD5 = 2,
		SMTPAUTH_LOGIN = 4,
		SMTPAUTH_PLAIN = 8
	};

	unsigned short authType = 0;

	bool auth = login.size() > 0 ? true : false;

	short family = OPTIONS_PROTO_FAMILY_IPV4;
	if (host.find(":") != dodoString::npos)
		family = OPTIONS_PROTO_FAMILY_IPV6;

	exchange ex;
	client net(family, OPTIONS_TRANSFER_TYPE_STREAM);

	net.connect(host, port, ex);

	dodoString mess;

	ex.readStreamString(mess);
	ex.writeStreamString("EHLO " + network::getLocalName() + "\r\n");
	ex.readStreamString(mess);

	if (string::stringToI(mess.substr(0, 3)) != 250)
		throw baseEx(ERRMODULE_TOOLSMISC, MISCEX_MAIL, ERR_LIBDODO, MISCEX_BADMAILHELO, TOOLSMISCEX_BADMAILHELO_STR, __LINE__, __FILE__);

	if (auth)
	{
		if (string::contains(mess, "CRAM-MD5"))
			addFlag(authType, SMTPAUTH_CRAMMD5);

		if (string::contains(mess, "LOGIN"))
			addFlag(authType, SMTPAUTH_LOGIN);

		if (string::contains(mess, "PLAIN"))
			addFlag(authType, SMTPAUTH_PLAIN);
	}

	if (auth)
	{
		if (isSetFlag(authType, SMTPAUTH_CRAMMD5))
		{
			ex.writeStreamString("AUTH CRAM-MD5\r\n");
			ex.readStreamString(mess);

			if (string::stringToI(mess.substr(0, 3)) != 334)
				throw baseEx(ERRMODULE_TOOLSMISC, MISCEX_MAIL, ERR_ERRNO, MISCEX_BADMAILAUTH, TOOLSMISCEX_BADMAILAUTH_STR, __LINE__, __FILE__);

			dodoString ticket = code::decodeBase64(mess.substr(4));

			dodoString md5pass;
			if (pass.size() > 64)
				md5pass = code::MD5(pass);
			else
				md5pass = pass;

			unsigned char ipad[65];
			unsigned char opad[65];

			memset(ipad, 0, 65);
			memset(opad, 0, 65);

			memcpy(ipad, md5pass.c_str(), md5pass.size());
			memcpy(opad, md5pass.c_str(), md5pass.size());

			for (short i = 0; i < 64; ++i)
			{
				ipad[i] ^= 0x36;
				opad[i] ^= 0x5c;
			}

			code::MD5_CTX context;
			unsigned char digest[16];

			code::MD5Init(&context);
			code::MD5Update(&context, ipad, 64);
			code::MD5Update(&context, (unsigned char *)ticket.c_str(), ticket.size());
			code::MD5Final(digest, &context);

			code::MD5Init(&context);
			code::MD5Update(&context, opad, 64);
			code::MD5Update(&context, digest, 16);
			code::MD5Final(digest, &context);

			md5pass = code::binToHex(dodoString((char *)&digest, 16));

			ex.writeStreamString(code::encodeBase64(login + " " + md5pass) + "\r\n");
			ex.readStreamString(mess);

			if (string::stringToI(mess.substr(0, 3)) != 235)
				throw baseEx(ERRMODULE_TOOLSMISC, MISCEX_MAIL, ERR_ERRNO, MISCEX_BADMAILAUTH, TOOLSMISCEX_BADMAILAUTH_STR, __LINE__, __FILE__);
		}
		else
		{
			if (isSetFlag(authType, SMTPAUTH_LOGIN))
			{
				ex.writeStreamString("AUTH LOGIN\r\n");
				ex.readStreamString(mess);

				if (string::stringToI(mess.substr(0, 3)) != 334)
					throw baseEx(ERRMODULE_TOOLSMISC, MISCEX_MAIL, ERR_ERRNO, MISCEX_BADMAILAUTH, TOOLSMISCEX_BADMAILAUTH_STR, __LINE__, __FILE__);

				ex.writeStreamString(code::encodeBase64(login) + "\r\n");
				ex.readStreamString(mess);

				if (string::stringToI(mess.substr(0, 3)) != 334)
					throw baseEx(ERRMODULE_TOOLSMISC, MISCEX_MAIL, ERR_ERRNO, MISCEX_BADMAILAUTH, TOOLSMISCEX_BADMAILAUTH_STR, __LINE__, __FILE__);

				ex.writeStreamString(code::encodeBase64(pass) + "\r\n");
				ex.readStreamString(mess);

				if (string::stringToI(mess.substr(0, 3)) != 235)
					throw baseEx(ERRMODULE_TOOLSMISC, MISCEX_MAIL, ERR_ERRNO, MISCEX_BADMAILAUTH, TOOLSMISCEX_BADMAILAUTH_STR, __LINE__, __FILE__);
			}
			else
			{
				if (isSetFlag(authType, SMTPAUTH_PLAIN))
				{
					ex.writeStreamString("AUTH PLAIN" + code::encodeBase64(login + "\0" + login + "\0" + pass) + "\r\n");
					ex.readStreamString(mess);

					if (string::stringToI(mess.substr(0, 3)) != 334)
						throw baseEx(ERRMODULE_TOOLSMISC, MISCEX_MAIL, ERR_ERRNO, MISCEX_BADMAILAUTH, TOOLSMISCEX_BADMAILAUTH_STR, __LINE__, __FILE__);
				}
			}
		}
	}

	ex.writeStreamString("MAIL FROM: <" + from + ">\r\n");
	ex.readStreamString(mess);

	dodoStringArray pock = explode(to, ",");

	dodoStringArray::iterator i = pock.begin(), j = pock.end();
	for (; i != j; ++i)
	{
		ex.writeStreamString("RCPT TO: <" + *i + ">\r\n");
		ex.readStreamString(mess);
	}

	ex.writeStreamString("DATA\r\n");
	ex.readStreamString(mess);

	ex.writeStreamString("To: " + to + "\r\n");
	ex.writeStreamString("From: " + from + "\r\n");
	ex.writeStreamString("X-Mailer: " PACKAGE_NAME "/" PACKAGE_VERSION "\r\n");
	ex.writeStreamString("Subject: " + subject  + "\r\n");
	ex.writeStreamString(headers);
	ex.writeStreamString(message);
	ex.writeStreamString("\r\n.\r\n");
	ex.writeStreamString("QUIT\r\n");
}

//-------------------------------------------------------------------
