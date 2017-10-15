


/********************************************************************
	created:	2011/09/20
	created:	20:9:2011   15:17
	filename: 	d:\Development\mrayEngine\Engine\mrayEngine\include\GZipCompress.h
	file path:	d:\Development\mrayEngine\Engine\mrayEngine\include
	file base:	GZipCompress
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/


#ifndef __GZipCompress__
#define __GZipCompress__

#include <vector>
#include <algorithm>

namespace mray
{
namespace Compress
{

class  GZipCompress
{
public:
	struct Chunk
	{
		unsigned char* data;
		int length;
		int pos;

		bool autoRemove;

		Chunk(bool autoRem=true)
		{
			autoRemove = autoRem;
			data = 0;
			length = 0;
			pos = 0;
		}
		~Chunk()
		{
			if (autoRemove)
				delete[] data;
		}

		bool eof()
		{
			return pos >= length;
		}
		int read(unsigned char* d, int len)
		{
			int l = std::min(len, length - pos);
			memcpy(d,data + pos, l);
			pos += l;
			return l;
		}
		int write(unsigned char* d, int len)
		{
			if (data == 0 || (length - pos) < len)
			{
				unsigned char* t = data;

				data = new unsigned char[len + pos];
				if (t)
				{
					memcpy(data, t, pos);
					delete[] t;
				}
				length = len + pos;
			}
			memcpy(data + pos, d, len);
			pos += len;
			return len;
		}
	};
private:
protected:
	static const int m_defaultChunk;
	int inputSize;
	int outputSize;
	std::vector<unsigned char> m_bytes;
	std::vector<unsigned char> m_outbytes;
	bool ReadChunk(Chunk* input);
public:

	GZipCompress();
	virtual~GZipCompress();

	virtual bool isLossless();

	virtual int compress(Chunk* input, Chunk* output);
	virtual int decompress(Chunk* input, Chunk* output);
};

}
}

#endif
