#ifndef ParametricPARAMETRIC_KEYFRAMECOLLECTION_HPP_
#define ParametricPARAMETRIC_KEYFRAMECOLLECTION_HPP_

#include "GeometricTransformation.hpp"

# include <map>
# include <array>

/**
 * \brief An ordered collection of keyframes.
 *
 * This class store the keyframes in ascending time order. For now, the
 * key frames only define a geometric transformation at a given time.
 * You can either extend this class to add other attributes to interpolate,
 * such as colors, or to create another class similar to this one.
 */
class BezierKeyframeCollection
{
public:
  /**
   * \brief Add a key frame to the collection.
   *
   * Add a key frame to the collection.
   * \param transformation The geometric transformation of the keyframe.
   * \param time The time of the keyframe.
   */
  void add( const GeometricTransformation& transformation, float time );



  GeometricTransformation computeBezier ( const float time_current, const float time_start, const float time_end,
    std::map< float, GeometricTransformation >::const_iterator & begin,
    std::map< float, GeometricTransformation >::const_reverse_iterator & end ) const;

  // TODO change doc
  // /**
  //  * \brief Interpolate a transformation at a given time with parametric function.
  //  *
  //  * This function will interpolate a geometric transformation at a given
  //  * time, using the stored keyframes and a polynom order. Since a geometric 
  //  * transformation is likely to be used as a mat4 to be send to the GPU, we 
  //  * return directly here the transformation in this representation.
  //  *
  //  * \param time Interpolation time
  //  * \param order Polynom Order
  //  * \return The interpolated geometric transformation.
  //  */
  glm::mat4 interpolateTransformation( float time_current, float time_start, float time_end ) const;


  /**
   * @brief Check if the collection is empty.
   * @return True if the collection is empty, false otherwise.
   */
  bool empty() const;

private:
  /**
   * \brief Definition of a keyframe.
   *
   * A keyframe is, for now, a geometric transformation at a given time.
   * This is represented as a pair, since it is exactly how it will
   * be stored inside m_keyframes (avoids conversions).
   */
  typedef std::pair< float, GeometricTransformation > Keyframe;

  /**
   * \brief Get the bounding keyframes at a specific time.
   *
   * This function get the keyframes that are bounding the given time.
   * They will be used to interpolate the transformation at this
   * specific time.
   * \param time Interpolation time.
   * \return The bounding keyframes stored in this collection.
   * \note We could return only an iterator here, but we thought
   * it might be easier to understand the function interpolateTransformation()
   * if the result is returned this way.
   */
  std::array< Keyframe, 2 > getBoundingKeyframes( float time ) const;

  /**
   * \brief Internal storage of the keyframes.
   *
   * Keyframes are stored inside a map. Such collection associate
   * to a time a geometric transformation. Its specificity is to
   * store the data in a way that it is trivial to iterate over the
   * keyframes in a increasing time order. Internally, a map element
   * is similar to a set of pair< float, GeometricTransformation >.
   */
  std::map< float, GeometricTransformation > m_keyframes;
};

# endif 
