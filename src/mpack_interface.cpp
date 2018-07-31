#include "mpack_interface.h"

bool mpack_nvm::PackData(const std::shared_ptr<nvmdata> dataToPack, uint8_t* packedDataBuffer, const size_t bufferSize)
{
	mpack_writer_t writer;

	mpack_writer_init(&writer, reinterpret_cast<char*>(packedDataBuffer), bufferSize);
	
	mpack_start_map(&writer, 3);
	mpack_write_cstr(&writer, "ver");
	mpack_write_u32(&writer, dataToPack->version);
	mpack_write_cstr(&writer, "lastWake");
	mpack_write_i32(&writer, dataToPack->lastWakeTime);
	mpack_write_cstr(&writer, "nextWake");
	mpack_write_i32(&writer, dataToPack->nextWakeTime);
	mpack_finish_map(&writer);
	// finish writing
	if (mpack_writer_destroy(&writer) != mpack_ok) {
	    return false;
	}

	return true;
}

std::shared_ptr<nvmdata> mpack_nvm::UnpackData(const uint8_t* packedDataBuffer, const size_t bufferSize)
	{
		std::shared_ptr<nvmdata> unpackedData = std::make_shared<nvmdata>();
		// parse a file into a node tree
		mpack_tree_t tree;
		mpack_tree_init(&tree, reinterpret_cast<const char*>(packedDataBuffer), bufferSize);
		mpack_node_t root = mpack_tree_root(&tree);
		// extract the example data on the msgpack homepage
		unpackedData->version = mpack_node_u32(mpack_node_map_cstr(root, "ver"));
		unpackedData->lastWakeTime = mpack_node_i32(mpack_node_map_cstr(root, "lastWake"));
		unpackedData->nextWakeTime = mpack_node_i32(mpack_node_map_cstr(root, "nextWake"));
		// clean up and check for errors
		if (mpack_tree_destroy(&tree) != mpack_ok) 
		{
		    return std::shared_ptr<nvmdata>();
		}

		return unpackedData;
	}
	