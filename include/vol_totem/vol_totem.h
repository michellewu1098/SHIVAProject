// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the VOL_TOTEM_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// VOL_TOTEM_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef VOL_TOTEM_EXPORTS
#define VOL_TOTEM_API __declspec(dllexport)
#else
#define VOL_TOTEM_API __declspec(dllimport)
#endif

#include <vector>

namespace totemio
{
//primitives
#define CONE_NODE_ID		1000
#define BOX_NODE_ID			1001
#define CYLINDER_NODE_ID	1002
#define ELLIPSOID_NODE_ID	1003
#define TORUS_NODE_ID		1004

#define CACHE_NODE_ID		1500

#define CSG_NODE_ID			2000
#define BLENDCSG_NODE_ID	2001
#define TRANSFORM_NODE_ID	2002

enum AnalysisCode
{
	CODE_UNDEFINED = 0,
	CODE_OK,
	CODE_MULTICOMPONENT,
	CODE_UNBALANCED,
	CODE_FAILURE
};

class VOL_TOTEM_API TotemNode {
public:
	TotemNode();
	~TotemNode();
	virtual unsigned int getTypeID() const;
	virtual unsigned int getChildrenSize() const;
	virtual TotemNode* getChild(unsigned int nIndex) const;

	bool addChild(TotemNode* pChild);
	const char * getID() const;

protected:
	class TotemNodeImpl;
	TotemNodeImpl * m_pImpl;
};

class VOL_TOTEM_API ConeNode : public TotemNode
{
public: 
	ConeNode(const char * pID);
	bool setConeParams(float radius, float height);
	bool getConeParams(float& radius, float& height) const;

private:
	float m_fRadius;
	float m_fHeight;
};

class VOL_TOTEM_API BoxNode : public TotemNode
{
public: 
	BoxNode(const char * pID);
	bool setBoxParams(float dimx, float dimy, float dimz);
	bool getBoxParams(float& dimx, float& dimy, float& dimz) const;

private:
	float m_dimensions[3];
};

class VOL_TOTEM_API CylinderNode : public TotemNode
{
public: 
	CylinderNode(const char * pID);
	bool setCylinderParams(float radiusX, float radiusY, float height);
	bool getCylinderParams(float& radiusX, float& radiusY, float& height) const;

private:
	float m_fRadiusX;
	float m_fRadiusY;
	float m_fHeight;
};

class VOL_TOTEM_API EllipsoidNode : public TotemNode
{
public: 
	EllipsoidNode(const char * pID);
	bool setEllipsoidParams(float radx, float rady, float radz);
	bool getEllipsoidParams(float& radx, float& rady, float& radz) const;

private:
	float m_radius[3];
};

class VOL_TOTEM_API TorusNode : public TotemNode
{
public: 
	TorusNode(const char * pID);
	bool setTorusParams(float radius_big, float radius_small);
	bool getTorusParams(float& radius_big, float& radius_small) const;

private:
	float m_radiusBig;
	float m_radiusSmall;
};

class VOL_TOTEM_API CSGNode : public TotemNode
{
public: 
	enum CSG_TYPE {
		CSG_TYPE_UNION,
		CSG_TYPE_INTERSECTION,
		CSG_TYPE_SUBTRACTION
	};
	CSGNode(const char * pID);
	virtual bool setCSGType(int type);
	virtual bool getCSGType(int& type) const;

protected:
	int m_nType;
};

class VOL_TOTEM_API BlendCSGNode : public CSGNode
{
public:
	BlendCSGNode(const char * pID);
	bool setBlendParams(float a0, float a1, float a2);
	bool getBlendParams(float& a0, float& a1, float& a2) const;

private:
	float m_fA0, m_fA1, m_fA2;
};

class VOL_TOTEM_API TransformNode : public TotemNode
{
public:
	TransformNode(const char * pID);
	
	bool setTransformParams(float tx, float ty, float tz, //translate, 0 by default
		float rx, float ry, float rz, //rotate as euler angles in radians, 0 by default
		float sx, float sy, float sz, //scale, 1 by default
		float refrx, float refry, float refrz, //reference point for rotation, 0 by default
		float refsx, float refsy, float refsz); //reference point for scale, 0 by default
	bool getTransformParams(float& tx, float& ty, float& tz, float& rx, float& ry, float& rz, 
		float& sx, float& sy, float& sz, float& refrx, float& refry, float& refrz, float& refsx, float& refsy, float& refsz) const; 

private:
	float m_params[15];
};

class VOL_TOTEM_API CacheNode : public TotemNode
{
public: 
	CacheNode(const char * pID);
	bool loadFromVol(const char * filename);

	bool getBoundingBox(float** o_bbox);
	bool generateCache(unsigned int resolution_x, unsigned int resolution_y, unsigned int resolution_z, float * i_bbox, float ** o_array); //i_bbox can be NULL, in this case we use default one
	
	//the next two functions for internal use only!
	bool loadFromFAPINode(void * pNode);
	void * getFAPIPointer() const;

private:
	void * m_pNode;
};

VOL_TOTEM_API bool openVol(const char * filename_to_load, TotemNode** o_node);
VOL_TOTEM_API bool saveVol(const char * filename_to_save, const TotemNode* pRootNode);

//analysis returns one of the AnalysisCode enum values
VOL_TOTEM_API unsigned int analyseModel(const TotemNode* pRootNode, int nDepth = 6); 

VOL_TOTEM_API void freePointer(float** io_pointer);
VOL_TOTEM_API void freeTree(TotemNode* io_node);
}