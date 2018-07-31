#pragma once
#include "mpack/mpack.h"

#include "ParticleWiring.h"
#include "Parameter.h"
#include <vector>
#include <tuple>

#include "nvmdata.h"


class mpack_nvm
{
public:
	// encode to memory buffer
	//	const size_t size = 256;//size of NVMbuffer

	//	char data[size]; 
	
	static bool PackData(const std::shared_ptr<nvmdata> dataToPack, uint8_t* packedDataBuffer, const size_t bufferSize);

	static std::shared_ptr<nvmdata> UnpackData(const uint8_t* packedDataBuffer, const size_t bufferSize);
	/*void UnpackData()
	{
		enum key_names       {KEY_COMPACT, KEY_SCHEMA, KEY_COUNT};
		const char* keys[] = {"compact"  , "schema"  };

		bool found[KEY_COUNT] = {0};
		bool compact = false;
		int schema = -1;
		for (size_t i = mpack_expect_map_max(&reader, 100); i > 0 && mpack_reader_error(&reader) == mpack_ok; --i) {
		    switch (mpack_expect_key_cstr(&reader, keys, found, key_count)) {
		        case KEY_COMPACT: compact = mpack_expect_bool(&reader); break;
		        case KEY_SCHEMA:  schema  = mpack_expect_int(&reader);  break;
		        default: mpack_discard(&reader); break;
		    }
		}
		// compact is not optional
		if (!found[KEY_COMPACT])
	    mpack_reader_flag_error(&reader, mpack_error_data);
	}*/

};