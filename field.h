#ifndef FIELD_H
#define FIELD_H

#include <../glm-0.9.8.5/glm/glm.hpp>
#include "structpile.h"

//-----------------------------------------------------------------------------
// Setting up logic to be able to determine the data type of the gradient
template <typename U>
struct GradType
{
   typedef int GType;
};

template<>
struct GradType<float>
{
   typedef glm::vec3 GType;
};

template<>
struct GradType<glm::vec3>
{
   typedef glm::mat3x3 GType;
};



//-----------------------------------------------------------------------------

template< typename U >
class Field
{
  public:

    Field(){}

   virtual ~Field(){}

   typedef U fieldDataType;
   typedef typename GradType<U>::GType fieldGradType;

   virtual const fieldDataType eval( const glm::vec3& P ) const { fieldDataType base; return base; }
   virtual const fieldGradType grad( const glm::vec3& P ) const { fieldGradType base; return base; }


};

typedef Field<float>* FieldFloatPtr;
typedef Field<color>* FieldColorPtr;
typedef Field<glm::vec3>* FieldVectorPtr;
typedef Field<glm::mat3x3>* FieldMatrixPtr;

#endif // FIELD_H
