#ifndef __COMMON_SCENE_GRAPH_H
#define __COMMON_SCENE_GRAPH_H


#include <boost/smart_ptr.hpp>
#include <list>
#include <string>
#include <map>
#include "CommonAlembic.h"

class SceneNode;
typedef boost::shared_ptr<SceneNode> SceneNodePtr;

typedef std::list<SceneNodePtr>::iterator SceneChildIterator;

class IJobStringParser;

//for runtime type identification
namespace SceneNodeClass
{
   enum typeE{
      FILE,
      FILE_ALEMBIC,
      APP,
      APP_MAX,
      APP_MAYA,
      APP_XSI
   };
};

class SceneNode
{
public:
   typedef std::map<std::string, bool> SelectionT;

   enum nodeTypeE{
      SCENE_ROOT,
      ETRANSFORM,// external transform (a parent of a geometry node)
      ITRANSFORM,// internal transform (all other transforms)
      CAMERA,
      POLYMESH,
      SUBD,
      SURFACE,
      CURVES,
      PARTICLES,
      LIGHT,
      UNKNOWN,
      NUM_NODE_TYPES
   };

   SceneNode* parent;
   std::list<SceneNodePtr> children;

   nodeTypeE type;
   std::string name;
   std::string dccIdentifier;
   bool selected;

   SceneNode():parent(NULL), type(NUM_NODE_TYPES), selected(false)
   {}

   SceneNode(nodeTypeE type, std::string name, std::string identifier):parent(NULL), type(type), name(name), dccIdentifier(identifier)
   {}
   //~SceneNode();

   virtual SceneNodeClass::typeE getClass() = 0;
   virtual bool isClass(SceneNodeClass::typeE type) = 0;
   virtual void print() = 0;
};


class IJobStringParser;
class SceneNodeAlembic;
class SceneNodeApp : public SceneNode
{
public:
   virtual bool replaceData(SceneNodePtr fileNode, const IJobStringParser& jobParams){ return false; }
   virtual bool addChild(SceneNodePtr fileNode, const IJobStringParser& jobParams, SceneNodePtr& newAppNode){ return false; }
   virtual SceneNodeClass::typeE getClass() = 0;
   virtual bool isClass(SceneNodeClass::typeE type);
   virtual void print() = 0;
};

class SceneNodeFile : public SceneNode
{
public:
   bool isMergedIntoAppNode;
   bool isAttachedToAppNode;

   SceneNodeFile(): isMergedIntoAppNode(false), isAttachedToAppNode(false)
   {}

   virtual SceneNodeClass::typeE getClass() = 0;
   virtual bool isClass(SceneNodeClass::typeE type);
   virtual bool isMerged();
   virtual void setMerged(bool bMerged);
   virtual bool isAttached();
   virtual void setAttached(bool bAttached);
   virtual bool isSupported() = 0;
   virtual void print() = 0;
};

class SceneNodeAlembic : public SceneNodeFile
{
public:

   Abc::IObject iObj;

	int numSamples;
	bool isConstant;
	bool isMeshPointCache;
	bool isMeshTopoDynamic;

   SceneNodeAlembic(Abc::IObject& obj):iObj(obj), numSamples(0), isConstant(false), isMeshPointCache(false), isMeshTopoDynamic(false)
   {}

   virtual SceneNodeClass::typeE getClass();
   virtual bool isClass(SceneNodeClass::typeE type);
   virtual bool isSupported();
   virtual Abc::IObject getObject();
   virtual void print();
};

void printSceneGraph(SceneNodePtr root, bool bOnlyPrintSelected);

bool hasExtractableTransform( SceneNode::nodeTypeE type );

void selectNodes(SceneNodePtr root, SceneNode::SelectionT selectionMap, bool bSelectParents, bool bChildren, bool bSelectShapeNodes);

//void filterNodeSelection(SceneNodePtr root, bool bExcludeNonTransforms);


#endif