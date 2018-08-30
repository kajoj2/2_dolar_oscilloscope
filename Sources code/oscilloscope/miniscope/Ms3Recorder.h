/** \file
	\brief Custom format recorder suitable for continuous data recording
*/
//---------------------------------------------------------------------------

#ifndef Ms3RecorderH
#define Ms3RecorderH

#include <System.hpp>
#include <vector>

struct S_DATAFRAME;
class Data;

class Ms3Recorder
{
public:
	enum MODE {
		MODE_IDLE = 0,
		MODE_WRITE,
		MODE_READ
	};
	enum ERR {
		ERR_NONE = 0,
		ERR_INTERNAL,
		ERR_OPEN_FILE,
		ERR_CREATE_FILE,
		ERR_WRITE_FILE,
		ERR_READ_FILE,
		ERR_NO_DATA,
		ERR_UNKNOWN_FMT,
		ERR_UNSUPPORTED_FMT,
		ERR_FILE_SIZE_TOO_SMALL,
		ERR_FILE_IO,
		ERR_MISSING_FILE_FOOTER,
		ERR_UNSUPPORTED_CHAN_COUNT,
		ERR_NO_MEM
	};	
private:
	Ms3Recorder();
	Ms3Recorder(const Ms3Recorder& source);
	Ms3Recorder& operator=(const Ms3Recorder&);
	static Ms3Recorder instance;

	enum MODE  mode;

	int OpenForReading(AnsiString filename);
	int OpenForWriting(AnsiString filename);
	int FinishRecording(void);

	/** \brief Vector if frame offsets inside file
	*/
	std::vector<unsigned int> file_idx;
	unsigned int file_size;

public:
	static Ms3Recorder& Instance() {
		return instance;
	}
	unsigned int GetFrameCount(void);
	/** \brief Get data frame from loaded file
		\param frame_id frame index (starting from 0) inside file
		\param desc frame parameters
		\param data vector to fill with samples
		\return 0 on success
	*/
	int GetFrame(unsigned int frame_id, Data& data);
	/** \brief Get data frame description from loaded file (actual samples are not loaded)
		\param frame_id frame index (starting from 0) inside file
		\return 0 on success
	*/
	int GetFrameDesc(unsigned int frame_id, Data& data);
	/** \brief Store frame to previously selected file
		\param data samples
		\return 0 on success
	*/
	int StoreFrame(const Data& data);
	/** \brief Specify file to write to, file to write from or
		close opened file
		\return 0 on success
	*/
	int SetMode(enum MODE mode, AnsiString filename="");
	/** \brief Get file size, either opened for reading or current size of file where
		data is stored
		\return file size in bytes
	*/
	unsigned int GetFileSize(void);
	enum MODE GetMode(void) const
	{
    	return mode;
	}

	/** \brief Check specified file for errors
	*/
	static enum ERR VerifyFile(AnsiString filename);
	/** \brief Fix missing file footer error
		\param filename input file to be fixed
		\param outputfile name of fixed file to be created
        \param progress progress feedback (0..100%)
		\return 0 on success
	*/
	static enum ERR FixMissingFileFooter(AnsiString filename,
		AnsiString outputfile, int &progress);
};

//---------------------------------------------------------------------------
#endif
