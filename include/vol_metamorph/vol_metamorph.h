// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the VOL_METAMORPH_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// VOL_METAMORPH_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef VOL_METAMORPH_EXPORTS
#define VOL_METAMORPH_API __declspec(dllexport)
#else
#define VOL_METAMORPH_API __declspec(dllimport)
#endif

#define METAMORPH_LINEAR	0
#define METAMORPH_STB		1

struct VOL_METAMORPH_API metamorph_params 
{
	int m_type; //see defines above
	float m_alpha; //time parameter
	float m_a0, m_a1, m_a2, m_a3, m_clipdist; //for space-time blend

	//anything else?
};

struct VOL_METAMORPH_API open_params
{
	bool m_fit_to_box;
	bool m_calculate_bbox;
	bool m_fill_array;
	bool m_exact_bbox;
};

VOL_METAMORPH_API bool openVol(const char * filename, unsigned int resolution_x, unsigned int resolution_y, unsigned int resolution_z, const open_params& params, float** o_bbox, float** o_array);
VOL_METAMORPH_API bool saveVol(const std::string& i_filename1, const std::string& i_filename2, const metamorph_params& params, const std::string& file_to_save);
VOL_METAMORPH_API void freePt(float** io_pointer);