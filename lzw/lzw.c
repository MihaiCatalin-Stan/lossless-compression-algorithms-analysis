#define LZW_EDDY_IMPLEMENTATION
// #define LZW_MAX_CODE_WIDTH 14
#include "lzw.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
#include <errno.h>

static const char *infile;
static const char *outfile;
static int compress = 0;
static size_t maxlen = 0;

static int parse_args(int argc, char **argv) {
	for (int i=1 ; i < argc ; ++i) {
		const char *arg = argv[i];
		// "argv[argc] shall be a null pointer", section 5.1.2.2.1
		const char *value = argv[i+1];

		if (arg && *arg == '-') {
			++arg;
			if (value) {
				switch (*arg) {
					case 'c':
						compress = 1;
						infile = value;
						break;
					case 'd':
						/* fallthrough */
					case 'x':
						compress = 0;
						infile = value;
						break;
					case 'o':
						outfile = value;
						break;
					case 'm':
						maxlen = atoi(value);
						break;
				}
			}
		}
	}

	return 0;
}

static void lzw_compress_file(const char *srcfile, const char *destfile) {
	FILE *ifile = fopen(srcfile, "rb");

	if (!ifile) {
		fprintf(stderr, "Error: %m\n");
		return;
	}
	fseek(ifile, 0, SEEK_END);
	long slen = ftell(ifile);
	fseek(ifile, 0, SEEK_SET);

	FILE *ofile = fopen(destfile, "wb");
	if (ofile) {
		uint8_t *src = malloc(slen);
		if (!src) {
			fprintf(stderr, "ERROR: memory allocation of %ld bytes failed.\n", slen);
			exit(1);
		}
		uint8_t dest[4096];

		struct lzw_state state = { 0 };
		if (maxlen > 0) {
			state.longest_prefix_allowed = maxlen;
			printf("WARNING: Restricting maximum prefix length to %zu.\n", state.longest_prefix_allowed);
		}

		if ((fread(src, slen, 1, ifile) != 1) && (ferror(ifile) != 0)) {
			fprintf(stderr, "fread '%s': %s", srcfile, strerror(errno));
			exit(EXIT_FAILURE);
		}

		ssize_t res, written = 0;
		while ((res = lzw_compress(&state, src, slen, dest, sizeof(dest))) > 0) {
			fwrite(dest, res, 1, ofile);
			written += res;
		}
		if (res == 0) {
			return;
		} else if (res < 0) {
			fprintf(stderr, "Compression returned error: %s (err: %zd)\n", lzw_strerror(res), res);
		}
		fclose(ofile);
		free(src);
	} else {
		fprintf(stderr, "Error: %m\n");
	}
	fclose(ifile);
}

static void lzw_decompress_file(const char *srcfile, const char *destfile) {
	FILE *ifile = fopen(srcfile, "rb");

	if (!ifile) {
		fprintf(stderr, "Error: %m\n");
		return;
	}
	fseek(ifile, 0, SEEK_END);
	long slen = ftell(ifile);
	fseek(ifile, 0, SEEK_SET);

	if (slen > 0) {
		FILE *ofile = stdout;
		if (strcmp(destfile, "-") != 0) {
			ofile = fopen(destfile, "wb");
		}
		if (ofile) {
			uint8_t dest[4096];
			size_t dest_len = sizeof(dest);
			if (maxlen > 0 && maxlen + 1 < dest_len) {
				dest_len = maxlen + 1;
				printf("WARNING: Restricting output buffer to %zu bytes.\n", dest_len);
			}
			uint8_t *src = malloc(slen);
			if (!src) {
				fprintf(stderr, "ERROR: memory allocation of %ld bytes failed.\n", slen);
				exit(1);
			}

			if ((fread(src, slen, 1, ifile) != 1) && (ferror(ifile) != 0)) {
				fprintf(stderr, "fread '%s': %s", srcfile, strerror(errno));
				exit(EXIT_FAILURE);
			}

			struct lzw_state state = { 0 };

			ssize_t res, written = 0;
			// Returns 0 when done, otherwise number of bytes written to destination buffer. On error, < 0.
			while ((res = lzw_decompress(&state, src, slen, dest, dest_len)) > 0) {
				fwrite(dest, res, 1, ofile);
				written += res;
			}
			if (res == 0) {
				return;
			} else if (res < 0) {
				fprintf(stderr, "Decompression returned error: %s (err: %zd)\n", lzw_strerror(res), res);
			}
			fclose(ofile);
			free(src);
		} else {
			fprintf(stderr, "Error: %m\n");
		}
	}
	fclose(ifile);
}

int main(int argc, char *argv []) {
	parse_args(argc, argv);

	if (!infile || !outfile) {
		printf("Usage: %s -c file|-d file -o outfile\n", argv[0]);
		printf("Compiled Configuration:\n LZW_MIN_CODE_WIDTH=%d, LZW_MAX_CODE_WIDTH=%d, LZW_MAX_CODES=%lu, sizeof(lzw_state)=%zu\n",
			LZW_MIN_CODE_WIDTH,
			LZW_MAX_CODE_WIDTH,
			LZW_MAX_CODES,
			sizeof(struct lzw_state)
		);
		return EXIT_SUCCESS;
	}

	if (compress) {
		lzw_compress_file(infile, outfile);
	} else {
		lzw_decompress_file(infile, outfile);
	}

	return EXIT_SUCCESS;
}
