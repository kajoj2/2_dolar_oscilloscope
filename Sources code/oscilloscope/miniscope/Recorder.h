/** \brief Data recorder (storing single frame)
*/

#ifndef RecorderH
#define RecorderH
//---------------------------------------------------------------------------

#include <string>
#include <vector>
#include <sys/timeb.h>

class Data;
struct S_DATAFRAME;

class Recorder
{
public:
	/** Storage format */
	enum E_FMT {
		FMT_JSON,	///< use JSON
		FMT_CSV		///< use CSV
	};

	/** \brief Store single data vector in file
		\param filename name of file to create or overwrite
		\param format data format for storage
		\param data actual data vector + metadata
		\return 0 on success
	*/
	static int Store(const std::string &filename, const enum E_FMT format, const Data &data);

private:
	static int StoreJson(const std::string &filename, const Data &data);
	static int StoreCsv(const std::string &filename, const Data &data);
};

#endif
