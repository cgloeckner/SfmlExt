#pragma once
#include <fstream>
#include <cstdint>
#include <string>
#include <vector>
#include <initializer_list>
#include <stdexcept>

template <typename T>
void read(std::ifstream& file, T& value) {
	file.read(reinterpret_cast<char*>(&value), sizeof(T));
}

template <typename T>
void write(std::ofstream& file, T const & value) {
	file.write(reinterpret_cast<char const*>(&value), sizeof(T));
}

/// Read several resources from an archive
/**
 *	Resource : resource to read from the binary stream
 *	Loader : loading algorithm based on char* and std::size_t
 *	Unpacker : unpacking algorithm (e.g. unzipping) to apply on the input stream (default: DummyPacking)
 *	LenghtType : integral type to save the key's length (default: uint8_t, means 0..255 string length)
 *	PosType : integral type for file offsets etc. (default: uint64_t)
 */
template <typename Resource, typename Loader, typename Unpacker, typename LengthType=std::uint8_t, typename PosType=std::uint64_t>
class ResourceArchiveReader {
	private:
		mutable std::ifstream file;
		Unpacker unpacker;
		Loader loader;
		std::unordered_map<std::string, PosType> index;

	public:
		bool open(std::string const & filename) {
			file.open(filename, std::ifstream::binary);
			if (!file.is_open()) {
				return false;
			}
			
			// read header
			PosType num;
			read(file, num);

			// create index
			for (PosType i = 0u; i < num; ++i) {
				// read resource key
				LengthType keylen;
				read(file, keylen);
				std::vector<char> buffer;
				buffer.resize(keylen);
				file.read(buffer.data(), keylen);
				std::string key{buffer.data(), keylen};
				// add to index
				auto current_pos = file.tellg();
				if (current_pos == -1) {
					// corrupted file
					throw std::range_error("corrupted resource archive");
				}
				index[key] = static_cast<PosType>(current_pos);
				// skip resource
				PosType res_size;
				read(file, res_size);
				file.seekg(res_size, std::ios::cur);
			}

			return true;
		}
		
		std::size_t size() const {
			return index.size();
		}
		
		bool has(std::string const & key) const {
			return index.find(key) != index.end();
		}

		std::unique_ptr<Resource> load(std::string const & key) const {
			// query index
			auto i = index.find(key);
			if (i == index.end()) {
				// resource not indexed
				return nullptr;
			}
			// go to start of resource
			file.seekg(i->second, file.beg);
			// read resource size
			PosType size;
			read(file, size);
			// read resource stream
			std::vector<char> buffer;
			buffer.resize(size);
			file.read(buffer.data(), size);
			// unpack data (inplace) and create resource
			unpacker(buffer.data(), size);
			return loader(buffer.data(), size);
		}
};

/// Write several resource files to an archive
/**
 *	Resource : resource to write to the binary stream
 *	Packer : packing algorithm (e.g. zipping) to apply on the output stream (default: DummyPacking)
 *	LenghtType : integral type to save the key's length (default: uint8_t, means 0..255 string length)
 *	PosType : integral type for file offsets etc. (default: uint64_t)
 */
template <typename Resource, typename Packer, typename LengthType=std::uint8_t, typename PosType=std::uint64_t>
struct ResourceArchiveWriter {
	bool save(std::vector<std::string> const & files, std::string const & filename) {
		std::ofstream file;
		Packer packer;

		file.open(filename, std::ofstream::binary | std::ofstream::trunc);
		if (!file.is_open()) {
			return false;
		}
	
		// write header
		write(file, static_cast<PosType>(files.size()));
	
		// write resources
		bool success{true};
		for (auto const & fname: files) {
			// open file
			std::ifstream tmp{fname, std::ifstream::binary};
			if (!tmp.is_open()) {
				success = false;
				break;
			}
			// determine size
			tmp.seekg(0u, tmp.end);
			auto file_size = static_cast<PosType>(tmp.tellg());
			tmp.seekg(0u, tmp.beg);
			// read content
			std::vector<char> buffer;
			buffer.resize(file_size);
			tmp.read(buffer.data(), file_size);
			tmp.close();
			// pack content
			packer(buffer.data(), file_size);
			// dump filename to archive
			write(file, static_cast<LengthType>(fname.size()));
			file.write(fname.data(), fname.size());
			// dump content to archive
			write(file, file_size);
			file.write(buffer.data(), file_size);
		}
	
		file.close();
		return success;
	}
};

struct DummyPacking {
	void operator()(char* buffer, std::uint64_t& size) const {
		// do nothing
	}
};

