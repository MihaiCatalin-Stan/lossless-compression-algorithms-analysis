/*
	MATRIX RLE Compressor/Decompressor
	Copyright (c) 2021-present cam900 and contributors
	See details on https://gitlab.com/cam900/RLE/-/blob/main/LICENSE
*/

#ifndef _LIBRLE_HPP
#define _LIBRLE_HPP

#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <stdint.h>

namespace librle
{
	typedef unsigned char u8;
	typedef unsigned short u16;
	typedef unsigned int u32;
	typedef unsigned long long u64;
	typedef signed char s8;
	typedef signed short s16;
	typedef signed int s32;
	typedef signed long long s64;

	class rle
	{
		public:
			// status
			enum librle_status : u8
			{
				LIBRLE_SUCCESS = 0,
				LIBRLE_FAILED_TO_OPEN_SOURCE,
				LIBRLE_FAILED_TO_OPEN_DESTINATION
			};

			// file case

			// compress: Compress file with RLE algorithm
			librle_status compress(bool drle, std::string inpath, std::string outpath, u64 *srcsize = nullptr, u64 *compressed_size = nullptr)
			{
				librle_status ret = librle_status::LIBRLE_SUCCESS;
				std::ifstream src(inpath, std::ifstream::binary);

				if (!src.is_open())
					ret = librle_status::LIBRLE_FAILED_TO_OPEN_SOURCE;
				else
				{
					std::ofstream dst(outpath, std::ios::binary | std::ios::out);
					if (dst.is_open())
					{
						u64 repval = 0;
						u8 curr = 0;
						s16 prev = -1;
						u8 dcur = 0;
						u8 dprv = 0;

						// get file length
						src.seekg(0, std::ios::end);
						std::streampos length = src.tellg();
						src.seekg(0, std::ios::beg);

						// Compress
						if (compressed_size != nullptr)
							(*compressed_size) = 0;
						while (src.tellg() < length)
						{
							if (drle)
							{
								read_from_file(src, dcur);
								curr = ((dcur - dprv) + 256) & 0xff;
								dprv = dcur;
							}
							else
							{
								read_from_file(src, curr);
							}
							if (curr != prev) // new value
							{
								if (repval > 0)
								{
									if (compressed_size != nullptr)
										(*compressed_size) += write_length(dst, repval - 1);
									else
										write_length(dst, repval - 1);

									repval = 0;
								}
								if (compressed_size != nullptr)
									(*compressed_size) += write_to_file(dst, curr);
								else
									write_to_file(dst, curr);
							}
							else // repeated value
							{
								if (repval <= 0)
								{
									if (compressed_size != nullptr)
										(*compressed_size) += write_to_file(dst, curr);
									else
										write_to_file(dst, curr);
								}
								repval += 1;
							}
							prev = curr;
						}

						// Flush RLE repeat
						if (repval > 0)
						{
							if (compressed_size != nullptr)
								(*compressed_size) += write_length(dst, repval - 1);
							else
								write_length(dst, repval - 1);

							repval = 0;
						}
						if (srcsize != nullptr)
							(*srcsize) = length;
						dst.close();
					}
					else
						ret = librle_status::LIBRLE_FAILED_TO_OPEN_DESTINATION;
					src.close();
				}
				return ret;
			}

			// decompress: Decompress already compressed file with RLE algorithm
			librle_status decompress(bool drle, std::string inpath, std::string outpath, u64 *srcsize = nullptr, u64 *decompressed_size = nullptr)
			{
				librle_status ret = librle_status::LIBRLE_SUCCESS;
				std::ifstream src(inpath, std::ifstream::binary);

				if (!src.is_open())
					ret = librle_status::LIBRLE_FAILED_TO_OPEN_SOURCE;
				else
				{
					std::ofstream dst(outpath, std::ios::binary | std::ios::out);
					if (dst.is_open())
					{
						u64 repval = 0;
						u8 curr = 0;
						s16 prev = -1;
						u8 dcur = 0;
						u8 buf = 0;

						// get file length
						src.seekg(0, std::ios::end);
						std::streampos length = src.tellg();
						src.seekg(0, std::ios::beg);

						// Decompress
						if (decompressed_size != nullptr)
							(*decompressed_size) = 0;
						while (src.tellg() < length)
						{
							if (drle)
							{
								read_from_file(src, dcur);
								curr = (dcur + curr) & 0xff;
							}
							else
							{
								read_from_file(src, curr);
							}
							buf = drle ? dcur : curr;
							if (decompressed_size != nullptr)
								(*decompressed_size) += write_to_file(dst, curr);
							else
								write_to_file(dst, curr);
							if (buf == prev) // repeated value
							{
								repval = read_length(src);
								if (repval > 0)
								{
									while (repval)
									{
										if (drle)
											curr += dcur;

										if (decompressed_size != nullptr)
											(*decompressed_size) += write_to_file(dst, curr);
										else
											write_to_file(dst, curr);

										repval--;
									}
								}
							}
							prev = buf;
						}
						if (srcsize != nullptr)
							(*srcsize) = length;
						dst.close();
					}
					else
						ret = librle_status::LIBRLE_FAILED_TO_OPEN_DESTINATION;
					src.close();
				}
				return ret;
			}

			// read_length: Read variable size length value from file
			u64 read_length(std::ifstream& src)
			{
				u8 in = 0;
				read_from_file(src, in); // fetch data from file
				if (in & 0x80) // multi byte length
				{
					u64 res = in & 0x7f;
					u8 adj = 7;
					while (in & 0x80) // continuous data?
					{
						read_from_file(src, in); // fetch next byte
						res += (u64(in & 0x7f) << adj) + (1ULL << adj); // add to result
						adj += 7;
					}
					return res;
				}
				return in;
			}

			// write_length: Write variable size length value from file
			u64 write_length(std::ofstream& dst, u64 in)
			{
				u8 outbyte;
				if (in < 0x80) // 0-7f : Single byte
				{
					outbyte = in & 0x7f;
					return write_to_file(dst, outbyte);
				}
				else // 80 or larger: Multi byte
				{
					u64 byte_num = 0;
					u64 tmp = 0x80;
					u64 prv = 0x80;
					u8 adj = 7;
					int siz = 0;
					while (in >= tmp)
					{
						prv = tmp;
						siz += 1;
						adj += 7;
						tmp += 1ULL << adj;
					}
					in -= prv;
					while (siz)
					{
						outbyte = 0x80 | (in & 0x7f);
						byte_num += write_to_file(dst, outbyte);
						siz -= 1;
						in = in >> 7;
					}
					outbyte = in & 0x7f;
					byte_num += write_to_file(dst, outbyte);
					return byte_num;
				}
			}

			// memory case
			librle_status compress(bool drle, u8 *src, u8 *dst, u64 srcsize, u64 *compressed_size = nullptr)
			{
				librle_status ret = librle_status::LIBRLE_SUCCESS;
				u64 pos = 0;
				u64 repval = 0;
				u8 curr = 0;
				s16 prev = -1;
				u8 dcur = 0;
				u8 dprv = 0;

				// Compress
				if (compressed_size != nullptr)
					(*compressed_size) = 0;
				while (pos < srcsize)
				{
					if (drle)
					{
						pos += read_from_memory(src, dcur);
						curr = ((dcur - dprv) + 256) & 0xff;
						dprv = dcur;
					}
					else
					{
						pos += read_from_memory(src, curr);
					}
					if (curr != prev) // new value
					{
						if (repval > 0)
						{
							if (compressed_size != nullptr)
								(*compressed_size) += write_length(dst, repval - 1);
							else
								write_length(dst, repval - 1);
							repval = 0;
						}
						if (compressed_size != nullptr)
							(*compressed_size) += write_to_memory(dst, curr);
						else
							write_to_memory(dst, curr);
					}
					else // repeated value
					{
						if (repval <= 0)
						{
							if (compressed_size != nullptr)
								(*compressed_size) += write_to_memory(dst, curr);
							else
								write_to_memory(dst, curr);
						}
						repval += 1;
					}
					prev = curr;
				}

				// Flush RLE repeat
				if (repval > 0)
				{
					if (compressed_size != nullptr)
						(*compressed_size) += write_length(dst, repval - 1);
					else
						write_length(dst, repval - 1);
					repval = 0;
				}
				return ret;
			}

			librle_status decompress(bool drle, u8 *src, u8 *dst, u64 srcsize, u64 *decompressed_size = nullptr)
			{
				librle_status ret = librle_status::LIBRLE_SUCCESS;
				u64 pos = 0;
				u64 repval = 0;
				u8 curr = 0;
				s16 prev = -1;
				u8 dcur = 0;
				u8 buf = 0;

				// Decompress
				if (decompressed_size != nullptr)
					(*decompressed_size) = 0;
				while (pos < srcsize)
				{
					if (drle)
					{
						pos += read_from_memory(src, dcur);
						curr = (dcur + curr) & 0xff;
					}
					else
					{
						pos += read_from_memory(src, curr);
					}
					buf = drle ? dcur : curr;
					if (decompressed_size != nullptr)
						(*decompressed_size) += write_to_memory(dst, curr);
					else
						write_to_memory(dst, curr);

					if (buf == prev) // repeated value
					{
						repval = read_length(src, pos);
						if (repval > 0)
						{
							while (repval)
							{
								if (drle)
									curr += dcur;

								if (decompressed_size != nullptr)
									(*decompressed_size) += write_to_memory(dst, curr);
								else
									write_to_memory(dst, curr);

								repval--;
							}
						}
					}
					prev = buf;
				}
				return ret;
			}

			// read_length: Read variable size length value from memory
			u64 read_length(u8 *src, u64 &pos)
			{
				u8 in = 0;
				pos += read_from_memory(src, in); // fetch data from memory
				if (in & 0x80) // multi byte length
				{
					u64 res = in & 0x7f;
					u8 adj = 7;
					while (in & 0x80) // continuous data?
					{
						pos += read_from_memory(src, in); // fetch next byte
						res += (u64(in & 0x7f) << adj) + (1ULL << adj); // add to result
						adj += 7;
					}
					return res;
				}
				return in;
			}

			// write_length: Write variable size length value from memory
			u64 write_length(u8 *dst, u64 in)
			{
				u8 outbyte;
				if (in < 0x80) // 0-7f : Single byte
				{
					outbyte = in & 0x7f;
					return write_to_memory(dst, outbyte);
				}
				else // 80 or larger: Multi byte
				{
					u64 byte_len = 0;
					u64 tmp = 0x80;
					u64 prv = 0x80;
					u8 adj = 7;
					int siz = 0;
					while (in >= tmp)
					{
						prv = tmp;
						siz += 1;
						adj += 7;
						tmp += 1ULL << adj;
					}
					in -= prv;
					while (siz)
					{
						outbyte = 0x80 | (in & 0x7f);
						byte_len += write_to_memory(dst, outbyte);
						siz -= 1;
						in = in >> 7;
					}
					outbyte = in & 0x7f;
					byte_len += write_to_memory(dst, outbyte);
					return byte_len;
				}
			}

		private:
			template<typename T> inline u64 read_from_file(std::ifstream &f, T &src)
			{
				const u64 size = sizeof(T);
				f.read(reinterpret_cast<char*>(&src), size);
				return size;
			}

			template<typename T> inline u64 write_to_file(std::ofstream &f, T src)
			{
				const u64 size = sizeof(T);
				f.write(reinterpret_cast<char*>(&src), size);
				return size;
			}

			template<typename T> inline u64 read_from_memory(T *mem, T &src)
			{
				const u64 size = sizeof(T);
				src = (*mem);
				mem += size;
				return size;
			}

			template<typename T> inline u64 write_to_memory(T *mem, T src)
			{
				const u64 size = sizeof(T);
				(*mem) = src;
				mem += size;
				return size;
			}
	};
};


#endif