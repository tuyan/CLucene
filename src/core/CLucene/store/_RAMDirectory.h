/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#ifndef _lucene_store_intl_RAMDirectory_
#define _lucene_store_intl_RAMDirectory_


#include "IndexInput.h"
#include "IndexOutput.h"
#include "RAMDirectory.h"
//#include "Lock.h"
//#include "Directory.h"
//#include "CLucene/util/VoidMap.h"
//#include "CLucene/util/Arrays.h"

CL_NS_DEF(store)

	class RAMFile:LUCENE_BASE {
	private:
		struct RAMFileBuffer:LUCENE_BASE {
			uint8_t* _buffer; size_t _len;
			RAMFileBuffer(uint8_t* buf = NULL, size_t len=0) : _buffer(buf), _len(len) {};
			~RAMFileBuffer() { _CLDELETE_LARRAY(_buffer); };
		};


		// TODO: Should move to ArrayList ?
		//CL_NS(util)::CLVector<uint8_t*,CL_NS(util)::Deletor::Array<uint8_t> > buffers;
		CL_NS(util)::CLVector<RAMFileBuffer*,CL_NS(util)::Deletor::Object<RAMFileBuffer> > buffers;


		int64_t length;
		RAMDirectory* directory;
		int64_t sizeInBytes;                  // Only maintained if in a directory; updates synchronized on directory

		// This is publicly modifiable via Directory::touchFile(), so direct access not supported
		uint64_t lastModified;


	public:
        DEFINE_MUTEX(THIS_LOCK)
				
        #ifdef _DEBUG
		const char* filename;
        #endif
		// File used as buffer, in no RAMDirectory
		RAMFile( RAMDirectory* directory=NULL );
		~RAMFile();
		
		// For non-stream access from thread that might be concurrent with writing
		int64_t getLength();
		void setLength( const int64_t _length );
		
		// For non-stream access from thread that might be concurrent with writing
		uint64_t getLastModified();
		void setLastModified( const uint64_t lastModified );
		
		uint8_t* addBuffer( const int32_t size );
		uint8_t* getBuffer( const int32_t index );
		size_t getBufferLen(const int32_t index) const { return buffers[index]->_len; }
		int32_t numBuffers() const;
		uint8_t* newBuffer( const int32_t size );
		
		int64_t getSizeInBytes() const;
	};


	class RAMIndexOutput: public IndexOutput {		
	protected:
		RAMFile* file;
		bool deleteFile;
		
		uint8_t* currentBuffer;
		int32_t currentBufferIndex;
		
		int32_t bufferPosition;
		int64_t bufferStart;
		int32_t bufferLength;
		
		void switchCurrentBuffer();
		void setFileLength();
				
	public:
		LUCENE_STATIC_CONSTANT(int32_t,BUFFER_SIZE=1024);
		
		RAMIndexOutput(RAMFile* f);
		RAMIndexOutput();
  	    /** Construct an empty output buffer. */
		virtual ~RAMIndexOutput();

		virtual void close();

		int64_t length() const;
        /** Resets this to an empty buffer. */
        void reset();
  	    /** Copy the current contents of this buffer to the named output. */
        void writeTo(IndexOutput* output);
        
    	inline void writeByte(const uint8_t b);
    	void writeBytes(const uint8_t* b, const int32_t len);

    	void seek(const int64_t pos);
    	
    	void flush();
    	
    	int64_t getFilePointer() const;
    	
		const char* getObjectName(){ return getClassName(); }
		static const char* getClassName(){ return "RAMIndexOutput"; }    	
    	
	};

  class RAMIndexInput:public IndexInput{				
	private:
		RAMFile* file;
		int64_t _length;
		
		uint8_t* currentBuffer;
		int32_t currentBufferIndex;
		
		int32_t bufferPosition;
		int64_t bufferStart;
		int32_t bufferLength;
		
		void switchCurrentBuffer();
		
	protected:
		/** IndexInput methods */
		RAMIndexInput(const RAMIndexInput& clone);
		
	public:
		LUCENE_STATIC_CONSTANT(int32_t,BUFFER_SIZE=RAMIndexOutput::BUFFER_SIZE);

		RAMIndexInput(RAMFile* f);
		~RAMIndexInput();
		IndexInput* clone() const;

		void close();
		int64_t length() const;
		
		inline uint8_t readByte();
		void readBytes( uint8_t* dest, const int32_t len );
		
		inline int64_t getFilePointer() const;
		
		void seek(const int64_t pos);
		
		const char* getDirectoryType() const;
		const char* getObjectName() const;
		static const char* getClassName();
	};

CL_NS_END
#endif
