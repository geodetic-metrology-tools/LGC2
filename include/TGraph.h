/*! A file for TGraph.h

	A singleton class used to find the shortest path of transformations  between two
	reference frame. 
  
    Patterns: a singleton  
   
    Copyright 2002 CERN EST/SU. All rights reserved.
*/
#ifndef SU_graph
#define SU_graph


#if _MSC_VER >= 1000
#pragma once
#pragma warning (disable:4786)
#endif

#include <string>
#include <map>
#include <vector>

#include <iostream>
#include <iomanip>

#include <stddef.h>
//using namespace std;

class TAGeoidModel;
class TReferenceEllipsoid;
class TAReferenceFrame;
class TVCoordinateSystem;

class TVRefFrameTransformation;


#include "TRefFrameWrapper.h"
#include "TTransformWrapper.h"

#include "DynamicPriorityQueue.h"

/*!\ingroup spatialobjects
	@{*/

//!A singleton class used to find the transformations  between two reference frame
class TGraph
{
public:

		/*!@typedef Graph Structure type definition*/
		//@{
    typedef std::map<int, TTransformWrapper, std::less<int> > Successor;
            typedef std::pair<TRefFrameWrapper, Successor> RefFrame;
			typedef std::vector<RefFrame> GraphType;
			typedef GraphType::iterator RefFrameIterator;
			typedef GraphType::const_iterator RefFrameConstIterator;
		//@}


				
		/*!@name Member Functions */
		//@{
			/*! Instance method to obtain a pointer to the TGraph uniq instance */
			static TGraph* getGraph();

		

			/*! Display of the TGraph structure */
			friend std::ostream& operator <<(std::ostream& os, TGraph& graph)
			{//dislay of vertices with successors
				for (unsigned int i = 0; i < graph.size(); ++i)
				{
					os << graph[i].first.getName() << " <";

					Successor::iterator startN = graph[i].second.begin(), endN = graph[i].second.end();
					while (startN != endN)
					{
						os << graph[(*startN).first].first.getName() << ' ';
						++startN;
					}
					os << ">\n";
				}
				return os;
			}


			/*! Return the transformation between two reference frames
				\return a vector of Reference Frame transformations */
			std::vector<TARefFrameTransformation*> getTransform(TRefFrameWrapper& from, TRefFrameWrapper& to);
		//@}	
	
		static void deleteGraph();	

protected:

	/*!@name Protected Member Functions */
	//@{
		/*! Default Constructor */
		TGraph();

		/*! Destructor */
		virtual ~TGraph();
	//@}


private:

	/*!@name Private Member Functions */
	//@{
		void	init();

		/*! Copy constructor*/
		TGraph( const TGraph& );

		/*! Copy Assigment Operator*/
		TGraph& operator=( const TGraph& );

		/*! Reference Frames Iterators*/
		RefFrameIterator begin() {return C.begin();}
		RefFrameIterator end() {return C.end();}

		/*! Index operator for reference frames*/
		RefFrame& operator[](int i) {return C[i];}

		/*! return the size of the graph*/
		size_t size() const {return C.size();}

		/*! insert a reference frame in the graph as a reference frame wrapper*/
		int insert(const TRefFrameWrapper& RF);
		/*! insert a reference frame in the graph as a reference frame pointer*/
		int insert(TAReferenceFrame* RF);

		/*! insert a reference frame tranformation (including source and destination reference frames) in the graph
			\param trans TVRefFrameTransformation*/
		void insert(TARefFrameTransformation* trans, TARefFrameTransformation* transInv);

		/*! find the shortest path from a reference frame to all the others
			\param trans vector of integer containing indexes of transformations
			\param pred vector of integer containing indexes of the transformation just before
			\param tranform
			\param start the index of the reference frame from which the tranformation is wanted*/
		void path(std::vector<int>& trans, std::vector<int>& pred, std::vector<TTransformWrapper>& transform, int start);

		/*! fill the last transformation object with the current transformation*/
		void fillLastTransformation(const std::string from, const std::string to, const std::vector<TARefFrameTransformation*> lastTransform);
	//@}


private:

	/*! a pointer to the unique instance of TGraph */
	static TGraph*						fGraph;

	GraphType							C;

	/*! last transformation asked for */
	std::vector<TARefFrameTransformation*>	fLastTransformation;

	/*!name of the source referenece frame of the last transformation */
	std::string								fLastFrom;

	/*!name of the destination referenece frame of the last transformation */
	std::string								fLastTo;
};
/*@}*/

#endif
