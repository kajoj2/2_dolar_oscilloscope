//---------------------------------------------------------------------------


#pragma hdrstop

#include "Ms3Recorder.h"
#include "device/Data.h"
#include "Log.h"
#include <stdio.h>
#include <sys\timeb.h>
#include <iostream>
#include <fstream>
#include <assert.h>

//---------------------------------------------------------------------------

#pragma package(smart_init)

namespace
{
	FILE *pFile = NULL;
	/** \brief Data file header, used to identify
		file and it's structure version
	*/
	struct S_HEADER
	{
		char desc[32];
		int version;
		S_HEADER()
		{
			memset (desc, 0, sizeof(desc));
			strcpy(desc, "Miniscope v3 data");
			version = 1;
		}
	} header;
	/** \brief Describes single data frame.
		File can contain many S_DATA structures
	*/
	typedef struct S_DATA
	{
		/** Constant-size part of data frame
		*/
		struct S_HDR
		{
			/** Total frame size. Redundant, but may allow easier data recovery
				from damaged files. */
			int size;
			/** Frame capture timestamp
			*/
			struct timeb timestamp;
			/** Sampling period */
			float fDivX;
			int trigger_type;
			int trigger_source;
			/** Samples count per channel
			*/
			unsigned int samples_count;
			/** Channel count in ch_id table data table
			*/
			unsigned int ch_count;
		} hdr;
		/** Channel identifiers table (order of of channels in data table)
		*/
		int ch_id[];
		/** Coupling type for each channel, order according to ch_id
		*/
		int coupling_type[];
		/** Actual data. Channels are ordered according to ch_id table.
			Channels data are not interleaved (i.e. there are all samples
			from first channel and then all samples from second channel).
		*/
		float data[];
	} S_DATA;
	/** \brief Last element of the file
		\note Data frames and footer are separated by array of data
		frames offsets. Size of this array = frame_count.
		This array allows fast file content browsing.
	*/
	struct S_INDEX_FOOTER
	{
		char desc[32];
		unsigned int frame_count;	///< total frame count in file
		S_INDEX_FOOTER()
		{
			memset (desc, 0, sizeof(desc));
			strcpy(desc, "Data IDX footer");
			frame_count = 0;
		}
	} footer;
}

Ms3Recorder Ms3Recorder::instance;

Ms3Recorder::Ms3Recorder()
{
	SetMode(MODE_IDLE);
}

int Ms3Recorder::SetMode(enum MODE mode, AnsiString filename)
{
	int status = 0;
	if (this->mode == MODE_WRITE)
	{
		status = FinishRecording();
		if (status)
		{
        	LOG(E_LOG_TRACE, "Ms3Recorder: Failed to finish recording with status %d", status);
		}
	}
	if (pFile)
	{
		fclose(pFile);
		pFile = NULL;
	}

	file_idx.clear();
	file_idx.reserve(8192);

	this->mode = MODE_IDLE;

	switch (mode)
	{
	case MODE_IDLE:
		break;
	case MODE_WRITE:
		status = OpenForWriting(filename);
		break;
	case MODE_READ:
		status = OpenForReading(filename);
		break;
	default:
		assert(0);
		return -1;
	}
	if (status == 0)
	{
		this->mode = mode;
	}
	return status;
}

int Ms3Recorder::OpenForWriting(AnsiString filename)
{
	assert(pFile == NULL);
	pFile = fopen(filename.c_str(), "wb");
	if (!pFile)
		return ERR_CREATE_FILE;
	file_size = 0;
	int count = fwrite(&header, sizeof(header), 1, pFile);
	if (count != 1)
	{
		fclose(pFile);
		pFile = NULL;
		return ERR_WRITE_FILE;
	}
	file_size += sizeof(header);
	return ERR_NONE;
}

int Ms3Recorder::StoreFrame(const Data& data)
{
	assert (mode == MODE_WRITE);
	
	struct S_DATA::S_HDR datahdr;
	datahdr.timestamp = data.timestamp;
	datahdr.fDivX = data.fOscDivX;
	datahdr.trigger_type = data.trigger_type;
	datahdr.trigger_source = data.trigger_source;
	datahdr.ch_count = data.GetEnabledChannelCount();
	if (datahdr.ch_count == 0)
		return ERR_NO_DATA;
	datahdr.samples_count = data.getEnabledChannelConst().dataOsc.size();
	if (datahdr.samples_count == 0)
		return ERR_NO_DATA;

	LOG(E_LOG_TRACE, "Ms3Recorder: store frame");

	struct S_DATA sdata;
	sdata.hdr.size = 0;	// just to get rid of unused variable warning
	datahdr.size = sizeof(struct S_DATA::S_HDR) +
		( datahdr.ch_count * (sizeof(*sdata.ch_id) + sizeof(*sdata.coupling_type) + (datahdr.samples_count * sizeof(float))));
    assert(pFile);
	long pos = ftell(pFile);
	if (pos < 0)
	{
		fclose(pFile);
		pFile = NULL;
		SetMode(MODE_IDLE);
		return ERR_WRITE_FILE;
	}
	file_idx.push_back(pos);

	int count = fwrite(&datahdr, sizeof(datahdr), 1, pFile);
	if (count != 1)
	{
		fclose(pFile);
		pFile = NULL;
		SetMode(MODE_IDLE);
		return ERR_WRITE_FILE;
	}
	file_size += sizeof(datahdr);

	// write channel id array
	for (unsigned int ch_id = 0; ch_id < data.GetChannelCount(); ch_id++)
	{
		if (data.getChannelConst(ch_id).bEnabled == false)
		{
        	continue;
		}
		count = fwrite(&ch_id, sizeof(ch_id), 1, pFile);
		if (count != 1)
		{
			fclose(pFile);
			pFile = NULL;
			SetMode(MODE_IDLE);
			return ERR_WRITE_FILE;
		}
		file_size += sizeof(ch_id);
	}

	// write coupling type array
	for (unsigned int ch_id = 0; ch_id < data.GetChannelCount(); ch_id++)
	{
		if (data.getChannelConst(ch_id).bEnabled == false)
		{
        	continue;
		}
		int coupling_type = data.getChannelConst(ch_id).iCouplingType;
		count = fwrite(&coupling_type, sizeof(coupling_type), 1, pFile);
		if (count != 1)
		{
			fclose(pFile);
			pFile = NULL;
			SetMode(MODE_IDLE);
			return ERR_WRITE_FILE;
		}
		file_size += sizeof(coupling_type);
	}

	for (unsigned int ch_id = 0; ch_id < data.GetChannelCount(); ch_id++)
	{
		if (data.getChannelConst(ch_id).bEnabled == false)
		{
        	continue;
		}
		// write actual data array
		const std::vector<float> &samples = data.getChannelConst(ch_id).dataOsc;
		assert(samples.size() == datahdr.samples_count);
		count = fwrite(&samples[0], datahdr.samples_count * sizeof(samples[0]), 1, pFile);
		if (count != 1)
		{
			fclose(pFile);
			pFile = NULL;
			SetMode(MODE_IDLE);
			return ERR_WRITE_FILE;
		}
		file_size += datahdr.samples_count * sizeof(samples[0]);
	}
	
	return ERR_NONE;
}

int Ms3Recorder::FinishRecording(void)
{
	assert(mode == MODE_WRITE);
	assert(pFile);
	if (file_idx.size() > 0)
	{
		int count = fwrite(&file_idx[0], file_idx.size() * sizeof(file_idx[0]), 1, pFile);
		if (count != 1)
		{
			fclose(pFile);
			pFile = NULL;
			return ERR_WRITE_FILE;
		}
		file_size += file_idx.size() * sizeof(file_idx[0]);
	}
	footer.frame_count = file_idx.size();
	int count = fwrite(&footer, sizeof(footer), 1, pFile);
	if (count != 1)
	{
		fclose(pFile);
		pFile = NULL;
		return ERR_WRITE_FILE;
	}
	file_size += sizeof(footer);

	return ERR_NONE;
}

unsigned long filesize(FILE *stream)
{
	unsigned long curpos, length;
	curpos = ftell(stream);
	if (fseek(stream, 0L, SEEK_END) != 0)
		return -1;
	length = ftell(stream);
	fseek(stream, curpos, SEEK_SET);
	return length;
}

int Ms3Recorder::OpenForReading(AnsiString filename)
{
	assert(pFile == NULL);
	pFile = fopen(filename.c_str(), "rb");
	if (!pFile)
		return ERR_OPEN_FILE;
	// check file size - must be at least header size + footer size
	file_size = filesize(pFile);
	if (file_size < (int)(sizeof (S_HEADER) + sizeof (S_INDEX_FOOTER)))
		return ERR_FILE_SIZE_TOO_SMALL;
	int status = fseek(pFile, 0, SEEK_SET);
	if (status != 0)
		return ERR_FILE_IO;
	int count = fread(&header, sizeof(header), 1, pFile);
	if (count != 1)
		return ERR_FILE_IO;
	struct S_HEADER hdr_default;
	if (strncmp(header.desc, hdr_default.desc, sizeof(header.desc)))
		return ERR_UNKNOWN_FMT;
	if (header.version != hdr_default.version)
		return ERR_UNSUPPORTED_FMT;

	unsigned long footer_pos = file_size - sizeof (S_INDEX_FOOTER);
	status = fseek(pFile, footer_pos, SEEK_SET);
	if (status != 0)
		return ERR_FILE_IO;
	count = fread(&footer, sizeof(footer), 1, pFile);
	if (count != 1)
		return ERR_FILE_IO;
	struct S_INDEX_FOOTER ftr_default;

	if (strncmp(footer.desc, ftr_default.desc, sizeof(footer.desc)))
		return ERR_MISSING_FILE_FOOTER;

	file_idx.push_back(0);	// just to be able to get size of this container element
	unsigned long footer_idx_pos = file_size - sizeof (S_INDEX_FOOTER) - (footer.frame_count * sizeof (file_idx[0]));
	file_idx.clear();
	status = fseek(pFile, footer_idx_pos, SEEK_SET);
	if (status != 0)
		return ERR_FILE_IO;
	unsigned int idx;
	for (unsigned int i=0; i<footer.frame_count; i++)
	{
		int count = fread(&idx, sizeof(idx), 1, pFile);
		if (count != 1)
			return ERR_FILE_IO;
		file_idx.push_back(idx);
	}
	LOG(E_LOG_TRACE, "Ms3Recorder: Found %d items in file index.", file_idx.size());
	return ERR_NONE;
}

int Ms3Recorder::GetFrame(unsigned int frame_id, Data& data)
{
	assert (mode == MODE_READ);
	if (mode != MODE_READ)
		return -1;
	assert (frame_id < file_idx.size());
	assert (pFile);

	int status = GetFrameDesc(frame_id, data);
	if (status != ERR_NONE)
		return status;

	// read data array
	float *buf = new float[data.buffer_size];
	if (!buf)
	{
		fclose(pFile);
		pFile = NULL;
		SetMode(MODE_IDLE);
		delete[] buf;
		return ERR_NO_MEM;
	}
	for (unsigned int i=0; i<data.GetChannelCount(); i++)
	{
        Data::Channel &channel = data.getChannel(i);
		channel.dataOsc.resize(data.buffer_size);
		if (data.buffer_size > 0)
		{
			int count = fread(buf, data.buffer_size * sizeof(buf[0]), 1, pFile);
			if (count != 1)
			{
				fclose(pFile);
				pFile = NULL;
				SetMode(MODE_IDLE);
				return ERR_READ_FILE;
			}
		}
		for (unsigned int i=0; i<data.buffer_size; i++)
		{
			channel.dataOsc[i] = buf[i];
		}
	}
	
	delete[] buf;
	return ERR_NONE;
}

int Ms3Recorder::GetFrameDesc(unsigned int frame_id, Data &data)
{
	assert (mode == MODE_READ);
	if (mode != MODE_READ)
		return -1;
	assert (frame_id < file_idx.size());
	assert (pFile);

	struct S_DATA::S_HDR datahdr;
	unsigned long pos = file_idx[frame_id];
	int status = fseek(pFile, pos, SEEK_SET);
	if (status != 0)
		return ERR_FILE_IO;
	int count = fread(&datahdr, sizeof(datahdr), 1, pFile);
	if (count != 1)
	{
		fclose(pFile);
		pFile = NULL;
		SetMode(MODE_IDLE);
		return ERR_READ_FILE;
	}

	data.timestamp = datahdr.timestamp;
	data.fOscDivX = datahdr.fDivX;
	data.trigger_type = datahdr.trigger_type;
	data.trigger_source = datahdr.trigger_source;
	data.buffer_size = datahdr.samples_count;
	data.SetChannelCount(datahdr.ch_count);

	// channel id array
	for (unsigned int i=0; i<datahdr.ch_count; i++)
	{
		int ch_id;
		count = fread(&ch_id, sizeof(ch_id), 1, pFile);
		if (count != 1)
		{
			fclose(pFile);
			pFile = NULL;
			SetMode(MODE_IDLE);
			return ERR_READ_FILE;
		}
		data.getChannel(i).id = ch_id;
	}

	// read coupling type array
	for (unsigned int i=0; i<datahdr.ch_count; i++)
	{
		int coupling_type;
		count = fread(&coupling_type, sizeof(coupling_type), 1, pFile);
		if (count != 1)
		{
			fclose(pFile);
			pFile = NULL;
			SetMode(MODE_IDLE);
			return ERR_WRITE_FILE;
		}
		data.getChannel(i).iCouplingType = coupling_type;
	}

	return ERR_NONE;
}

unsigned int Ms3Recorder::GetFrameCount(void)
{
	assert (mode == MODE_READ);
	assert (pFile);
	return file_idx.size();
}

unsigned int Ms3Recorder::GetFileSize(void)
{
	if (mode == MODE_IDLE)
		return 0;
	return file_size;
}

enum Ms3Recorder::ERR Ms3Recorder::VerifyFile(AnsiString filename)
{
	FILE *pFile;
	struct S_HEADER header;
	struct S_INDEX_FOOTER footer;

	pFile = fopen(filename.c_str(), "rb");
	if (!pFile)
		return ERR_OPEN_FILE;
	// check file size - must be at least header size + footer size
	unsigned long file_size = filesize(pFile);
	if (file_size < (int)(sizeof (S_HEADER) + sizeof (S_INDEX_FOOTER)))
	{
		fclose(pFile);
		return ERR_FILE_SIZE_TOO_SMALL;
	}
	int status = fseek(pFile, 0, SEEK_SET);
	if (status != 0)
	{
		fclose(pFile);
		return ERR_FILE_IO;
	}
	int count = fread(&header, sizeof(header), 1, pFile);
	if (count != 1)
	{
		fclose(pFile);
		return ERR_FILE_IO;
	}
	struct S_HEADER hdr_default;
	if (strncmp(header.desc, hdr_default.desc, sizeof(header.desc)))
	{
		fclose(pFile);
		return ERR_UNKNOWN_FMT;
	}
	if (header.version != hdr_default.version)
	{
		fclose(pFile);
		return ERR_UNSUPPORTED_FMT;
	}

	unsigned long footer_pos = file_size - sizeof (S_INDEX_FOOTER);
	status = fseek(pFile, footer_pos, SEEK_SET);
	if (status != 0)
	{
		fclose(pFile);
		return ERR_FILE_IO;
	}
	count = fread(&footer, sizeof(footer), 1, pFile);
	if (count != 1)
	{
		fclose(pFile);
		return ERR_FILE_IO;
	}
	struct S_INDEX_FOOTER ftr_default;

	if (strncmp(footer.desc, ftr_default.desc, sizeof(footer.desc)))
	{
		fclose(pFile);
		return ERR_MISSING_FILE_FOOTER;
	}
	// everything looks fine
	fclose(pFile);
	return ERR_NONE;
}

enum Ms3Recorder::ERR Ms3Recorder::FixMissingFileFooter(AnsiString filename,
	AnsiString outputfile, int &progress)
{
	/** \todo Ms3Recorder::FixMissingFileFooter works not so fast */
	// assumption: type of error was verified earlier, we can assume file header is ok
    progress = 0;
	std::ifstream ifile;
    unsigned int length;
	ifile.exceptions ( ifstream::eofbit | ifstream::failbit | ifstream::badbit );
	std::ofstream ofile;
	ofile.exceptions ( ifstream::eofbit | ifstream::failbit | ifstream::badbit );
	try
	{
		ifile.open(filename.c_str(), ios::in | ios::binary);
		// get length of file:
		ifile.seekg (0, ios::end);
		length = ifile.tellg();
		ifile.seekg (0, ios::beg);
	}
	catch(...)
	{
		return ERR_OPEN_FILE;
	}
	try
	{
		ofile.open(outputfile.c_str(), ios::out | ios::trunc | ios::binary);
	}
	catch(...)
	{
		return ERR_CREATE_FILE;
	}

	try
	{
		// copy headers
		struct S_HEADER header;
		ifile.read((char*)&header, sizeof(header));
		ofile.write((char*)&header, sizeof(header));
	}
	catch(...)
	{
    	return ERR_FILE_IO;
	}

	// copy data
	struct S_DATA::S_HDR datahdr;
	int *ch_id = NULL;
	int *coupling_type = NULL;
	float *data = NULL;
	std::vector<unsigned int> file_idx;

	try
	{
		for(;;)
		{
#if 0
			ifile.read((char*)&datahdr, sizeof(datahdr));
			ch_id = (int*)realloc(ch_id, sizeof(*ch_id) * datahdr.ch_count);
			coupling_type = (int*)realloc(coupling_type, sizeof(*coupling_type) * datahdr.ch_count);
			data = (float*)realloc(data, sizeof(*data) * datahdr.samples_count);

			ifile.read((char*)ch_id, sizeof(*ch_id) * datahdr.ch_count);
			ifile.read((char*)coupling_type, sizeof(*coupling_type) * datahdr.ch_count);
			ifile.read((char*)data, sizeof(*data) * datahdr.samples_count);

			try
			{
				file_idx.push_back(ofile.tellp());
				ofile.write((char*)&datahdr, sizeof(datahdr));
				ofile.write((char*)ch_id, sizeof(*ch_id) * datahdr.ch_count);
				ofile.write((char*)coupling_type, sizeof(*coupling_type) * datahdr.ch_count);
				ofile.write((char*)data, sizeof(*data) * datahdr.samples_count);
			}
#else		// should be slightly faster
			ifile.read((char*)&datahdr, sizeof(datahdr));
			data = (float*)realloc(data, sizeof(*data) * datahdr.samples_count + datahdr.ch_count * (sizeof(*ch_id) + sizeof(*coupling_type)));
			ifile.read((char*)data, sizeof(*data) * datahdr.samples_count + datahdr.ch_count * (sizeof(*ch_id) + sizeof(*coupling_type)));

			try
			{
				file_idx.push_back(ofile.tellp());
				ofile.write((char*)&datahdr, sizeof(datahdr));
				ofile.write((char*)data, sizeof(*data) * datahdr.samples_count + datahdr.ch_count * (sizeof(*ch_id) + sizeof(*coupling_type)));
			}
#endif
			catch(...)
			{
            	return ERR_FILE_IO;
			}
			progress = (float)ifile.tellg()/length * 100;
		}
	}
	catch(...)
	{

	}

	if (ch_id)
		free(ch_id);
	if (coupling_type)
		free(coupling_type);
	if (data)
		free(data);

	try
	{
		ofile.write((char*)(&file_idx[0]), file_idx.size() * sizeof(file_idx[0]));
		struct S_INDEX_FOOTER footer;
		//memset(&footer, 0, sizeof(footer)); 
		footer.frame_count = file_idx.size();
		ofile.write((char*)&footer, sizeof(footer));
        progress = 100;
		return ERR_NONE;
	}
	catch(...)
	{
    	return ERR_FILE_IO;
	}
}

