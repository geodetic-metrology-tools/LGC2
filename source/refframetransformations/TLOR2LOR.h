#ifndef TLOR2LOR_H
#define TLOR2LOR_H


#include <algorithm>
#include <String>
#include <tree.h>

#include "TLGCData.h"

#define assert4D(x) assert((x) >= 0 && (x) < 4)

typedef std::shared_ptr<TTreeEntry> TDataSPtr;
// Using the tree implementatin from http://tree.phi-sci.com (stored in tree.h) 
// but manually added the namspace peka (had none before) derived from author's name.
typedef peka::tree<TDataSPtr> TDataTree;
typedef TDataTree::iterator TDataTreeIterator;
typedef peka::tree_node_<TDataSPtr>  Node;

/*!
	Enables transformations between local object reference systems.
*/
class TLOR2LOR{
	public:

		/*!
			Constructor

			\param tree Input tree structure consisting of nodes which represent local object reference systems.
			\param from Name of the transformation which led to the source node.
			\param to Name of the transformation which led to the destination node.
			\param name Name for the transformation which we establish between the source and destination nodes.
		*/
		TLOR2LOR(const TDataTree& tree, const std::string& from, const std::string& to, const std::string& name);

		/// Copy Constructor 
		TLOR2LOR(const  TLOR2LOR &lor2lor);

		/// Transforms the point by this transformation
		TspatialPoint& transform(TspatialPoint& p);
		
		/*!
			Partial derivatives with respect to X0.

			\param[in] pos Coordinate of the point for which we want the derivatives, point is in homogeneous coordinates (0[xi], 1[yi], 2[zi], 3[wi]).

			\returns Partial derivative with respect to X0 of the pos coordinate.
		*/
		inline TReal partDerivX0(int pos){
			return trafo.partDerivX0(pos);
		}
		/*!
			Partial derivatives with respect to Y0.

			\param[in] pos Coordinate of the point for which we want the derivatives, point is in homogeneous coordinates (0[xi], 1[yi], 2[zi], 3[wi]).

			\returns Partial derivative with respect to Y0  of the pos coordinate.
		*/
		inline TReal partDerivY0(int pos){
			return trafo.partDerivY0(pos);
		}
		/*!
			Partial derivatives with respect to Z0.

			\param[in] pos Coordinate of the point for which we want the derivatives, point is in homogeneous coordinates (0[xi], 1[yi], 2[zi], 3[wi]).

			\returns Partial derivative with respect to Z0 of the pos coordinate.
		*/
		inline TReal partDerivZ0(int pos){
			return trafo.partDerivZ0(pos);
		}
	
		/*!
			Partial derivatives with respect to angles.

			\param[in] trafoName Name of the transformation in the transformation chain which we want to differentiate.
			\param[in] pos Coordinate of the point for which we want the derivatives, point is in homogeneous coordinates (0[xi], 1[yi], 2[zi], 3[wi]).
			\param[in] p Point which is transformed.
			\param[in] angle Specification of the the angle (0 - omega (x-axis), 1 - phi (y-axis), 2 - kappa (z-axis)). With respect to this angle is returned partial derivative.

			\returns Partial derivative of the "pos" coordinate with respect to the "angle".
		*/
		TReal partialDerivativesAngle(const std::string& trafoName, int pos, const TspatialPoint& p, int angle);
		
		/*!
			Partial derivatives with respect to translations.

			\param[in] trafoName Name of the transformation in the transformation chain which we want to differentiate.
			\param[in] pos Coordinate of the point for which we want the derivatives, point is in homogeneous coordinates (0[xi], 1[yi], 2[zi], 3[wi]).
			\param[in] p Point which is transformed.
			\param[in] translation Specification of the translation part (0[t1], 1[t2], 2[t3]). With respect to this part is returned partial derivative.

			\returns Partial derivative of the "pos" coordinate with respect the "translation".
		*/
		TReal partialDerivativesTranslation(const std::string& trafoName, int pos, const TspatialPoint& p, int translation);

		/*!
			Partial derivatives with respect to scale factor.

			\param[in] trafoName Name of the transformation in the transformation chain which we want to differentiate.
			\param[in] pos Coordinate of the point for which we want the derivatives, point is in homogeneous coordinates (0[xi], 1[yi], 2[zi], 3[wi]).
			\param[in] p Point which is transformed.

			\returns Partial derivative of the "pos" coordinate with respect to the scale factor.
		*/
		TReal partialDerivativesScale(const std::string& trafoName, int pos, const TspatialPoint& p);

		/*!
			\returns Source Node, i.e. Node of the tree from which we fransform.
		*/
		inline Node getFromNode() const {return fFromNode;};

		/*!
			\returns Destination Node of the tree, i.e. Node of the tree to which we transform.
		*/
		inline Node getToNode() const {return fToNode;};

		/*!
			\returns Transformation which leads from "from" Node to "to" Node (this transformation was constructed by composing transformations on the path between these nodes).
		*/
		inline TLocalTransformation getTransformation() const{return trafo;};

		/*!
			\returns Total number of transformations on the path.
		*/
		inline int getNumberOfTransformations(){return transformationChain.size();}

		/*!
			\returns Transformation chain of the transformation.
		*/
		inline std::vector<TLocalTransformation> getTransformationChain() const{return transformationChain;}

	private:
		///Source node of the transformation chain.
		Node fFromNode;
		///Destination node of the transformation chain.
		Node fToNode;

		///Transformation chain between source and destination nodes.
		std::vector<TLocalTransformation> transformationChain;

		///Transformation between source and destination nodes.
		TLocalTransformation trafo;
		
		///Initialize the transformation, locates "from" and "to" Nodes in the tree, finds shortest path between them and sets up the "trafo"
		void initialize(const TDataTree& tree, const std::string& from, const std::string& to);

		/*! 
			Returns a position of given transformation in the "transformationChain"
			\throws Throws a logic_error if transformation was not found in the transformationChain.
		*/
		int getTransformationPosition(const std::string& trafoName);
};

#endif