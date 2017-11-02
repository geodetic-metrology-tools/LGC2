/*
© Copyright CERN 2000-2017. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef TLOR_2_LOR_H
#define TLOR_2_LOR_H


//LGC
#include <TTransformation.h>
#include <Global.h>

class TAdjustableHelmertTransformation;

/*!
	\ingroup LocalTransformations
	\brief Enables transformations between local object reference systems. 
	Provides methods for obtaining partial derivatives with respect to transformation parameters of any transformation.
*/
class TLOR2LOR{
	public:
		/*!
			Structure keeping pointer to an adjustable transformation (adjTrafo) and transformation (trafo) between two nodes.
			Information telling if this transormation is direct or inverse is stored in a (direct) variable.
		*/
		struct TransformAndParams{
			TAdjustableHelmertTransformation* adjTrafo; // Pointer to an adjustable transformation stored in a tree
			std::shared_ptr<TTransformation>  trafo;	// Transformation (direct or inverse) between two nodes, constructed from the adjTrafo transformation parameters.
			bool direct;	// TRUE means it is a direct transformation, FALSE means inverse 
		};
	/*!@name Constructors / Destructor*/
	//@{
		/*!
			\brief Constructor.

			\param tree Input tree structure consisting of nodes which represent local object reference system.
			\param from Name of the transformation stored in the source node
			\param to Name of the transformation stored in the destination node.
			\param name Name of the transformation to be established.
		*/
		TLOR2LOR(const TDataTree& tree, const std::string& from, const std::string& to, const std::string& name);

		/*!
			\brief Constructor.

			\param from Iterator set on the source node.
			\param to Iterator set on the destination node.
			\param name Name of the transformation to be established.
		*/
		TLOR2LOR(TDataTreeIterator from, TDataTreeIterator to, const std::string& name);

		/// Destructor
		~TLOR2LOR(){};
	//@}

		/// Transforms the point by this transformation
		TPositionVector& transform(TPositionVector& p) const;
		
		/*!
			\brief Partial derivatives with respect to x0 (x coordinate of the point in the original LOR (local object reference)) .

			\param[in] pos Coordinate of the transformed point of which we want the derivatives, point is in homogeneous coordinates (0[X], 1[Y], 2[Z], 3[W]). 

			The partial derivative obtained is therefore: 

				pos = 0 =>  (d X)/(d x0)
				pos = 1 => (d Y)/(d x0)
				pos = 2 => (d Z)/(d x0)

			\returns Partial derivative with respect to x0 of the 'pos' coordinate of the point.
		*/
		TReal partDerivWRespToX0(int pos) const;

		///	Return Partial derivatives with respect to X0 as a FreeVector.
		TFreeVector partDerivWRespToX0() const;


		/*!
			\brief Partial derivatives with respect to y0 (y coordinate of the point in the original LOR (local object reference)) .

			\param[in] pos Coordinate of the transformed point of which we want the derivatives, point is in homogeneous coordinates (0[X], 1[Y], 2[Z], 3[W]). 

			The partial derivative obtained is therefore: 
			
				pos = 0 =>  (d X)/(d y0)
				pos = 1 => (d Y)/(d y0)
				pos = 2 => (d Z)/(d y0)

			\returns Partial derivative with respect to y0 of the 'pos' coordinate of the point.
		*/
		TReal partDerivWRespToY0(int pos) const;



		///	Return Partial derivatives with respect to Y0 as a FreeVector.
		TFreeVector partDerivWRespToY0() const;

		/*!
			\brief Partial derivatives with respect to z0 (z coordinate of the point in the original LOR (local object reference)) .

			\param[in] pos Coordinate of the transformed point of which we want the derivatives, point is in homogeneous coordinates (0[X], 1[Y], 2[Z], 3[W]). 

			The partial derivative obtained is therefore: 

				pos = 0 =>  (d X)/(d z0)
				pos = 1 => (d Y)/(d z0)
				pos = 2 => (d Z)/(d z0)

			\returns Partial derivative with respect to z0 of the 'pos' coordinate of the point.
		*/
		TReal partDerivWRespToZ0(int pos) const;
	

		///	Return Partial derivatives with respect to Z0 as a FreeVector.
		TFreeVector partDerivWRespToZ0() const;

		/*!
			\brief Partial derivatives with respect to angles.

			\param[in] transfoName Name of the transformation in the transformation chain which we want to differentiate.
			\param[in] p Point which is transformed.
			\param[in] angle Specification of the the angle (0 - omega (x-axis), 1 - phi (y-axis), 2 - kappa (z-axis)). With respect to this angle is returned partial derivative.

			\returns Partial derivatives of the point 'p' with respect to the "angle".
		*/
		TFreeVector partialDerivativesAngle (const std::string& transfoName, const TPositionVector& p, int angle) const;
		
		/*!
			\brief Partial derivatives with respect to translations.

			\param[in] transfoName Name of the transformation in the transformation chain which we want to differentiate.
			\param[in] p Point which is transformed.
			\param[in] translation Specification of the translation part (0[t1], 1[t2], 2[t3]). With respect to this part is returned partial derivative.

			\returns Partial derivatives of the point 'p' with respect to the "translation".
		*/
		TFreeVector partialDerivativesTranslation(const std::string& transfoName, const TPositionVector& p, int translation) const;

		/*!
			\brief Partial derivatives with respect to scale factor.

			\param[in] transfoName Name of the transformation in the transformation chain which we want to differentiate.
			\param[in] p Point which is transformed.

			\returns Partial derivatives of the point 'p' with respect to the scale factor.
		*/
		TFreeVector partialDerivativesScale(const std::string& transfoName, const TPositionVector& p) const;

		/// Returns Source Node, the node of the tree from which is transformed.
		inline TDataTreeIterator getFromNode() const {return fFromNode;};

		/// Returns Destination Node of the tree, the node of the tree to which we transform.
		inline TDataTreeIterator getToNode() const {return fToNode;};
		
		/// Returns Transformation which leads from "from" Node to "to" Node (this transformation was constructed by composing transformations on the path between these nodes).
		inline TTransformation* getTransformation() {return &transfo;};

		/// Returns Total number of transformations on the path.
		inline int getNumberOfTransformations(){return int(transformationChain.size());}

		/// Returns Returns the transformation chain of the transformation, containing also the adjustable information.
		inline const std::vector<TransformAndParams>& getTransformationChain() const{return transformationChain;}

		/// Returns Returns the overall transformation between nodes.
		inline const TTransformation& getLOR2LORTransformation() const{return transfo;}

		/// Returns the name of this transformation
		inline std::string getName() const{return fName;}

		/*!
			\brief Updates the transformation chain (transformationChain) and the overall transformation (transfo). 

			Needs to be called when the parameters of some of the transformations in the tree have changed, typically after every LS iteration.
		*/
		void updateTree();

	private:

		///Source node of the transformation chain.
		TDataTreeIterator fFromNode;
		///Destination node of the transformation chain.
		TDataTreeIterator fToNode;
		///Name of this transformation.
		std::string fName;

		//Vector of everything what is needed for calculation partial derivatives and transforming.
		std::vector<TransformAndParams> transformationChain;
		///Transformation between source and destination nodes.
		TTransformation transfo;

		//ID of a node in which we stop going up from "from" and start going down to "to". If going up inverse transformation taken.
		std::vector<int> fTurningPoint;
		bool			 fTurningPointLocated;

		/*! Returns the comulative matrix which multiplies from left the matrix of partial derivatives (pre-multiplication)*/
		TTransformation getCumulativeBegin(int positionInChain)const;
		/*! Returns the comulative matrix which should multiply from right the matrix of partial derivatives (post-multiplication)*/
		TTransformation getCumulativeEnd(int positionInChain)const;

		/*! 
			Returns a position of given transformation in the "transformationChain"
			\throws Throws a logic_error if transformation was not found in the transformationChain.
		*/
		int getTransformationPosition(const std::string& transfoName) const;

		///Initialize the transformation, finds shortest path between the two nodes and sets up the transformation between them.
		void initialize();

		/// update the transformations
		void updateTransformation();
	
		/// Locate the 'from' and the 'to' node in the 'tree'
		void locateNodes(const TDataTree& tree, const std::string& from, const std::string& to);

		/// Locate the turning point, i.e. the break point, we stop going up and start goping down
		void locateTurningPoint();
};

#endif
