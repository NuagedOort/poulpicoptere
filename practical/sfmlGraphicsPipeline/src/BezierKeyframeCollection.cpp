# include "./../include/BezierKeyframeCollection.hpp"
# include <glm/gtx/compatibility.hpp>

void BezierKeyframeCollection::add( const GeometricTransformation& transformation, float time )
{
    m_keyframes.insert( std::make_pair(time, transformation) );
}

glm::mat4 BezierKeyframeCollection::computeBezier ( float time,
    std::map< float, GeometricTransformation >::const_iterator & begin,
    std::map< float, GeometricTransformation >::const_iterator & end ) const {

    //If given keyframes are the last adjacent two, return a linear interpolation
    if(std::next(begin) == end ){

        float factor = (time - begin->first)/(end->first - begin->first);
        //Interpolate each transformation component of the surrounding keyframes: orientation, translation, scale
        glm::quat orientation = glm::quat(glm::slerp(begin->second.getOrientation(),end->second.getOrientation(),factor));
        glm::vec3 translation = glm::vec3(glm::lerp(begin->second.getTranslation(),end->second.getTranslation(),factor));
        glm::vec3 scale = glm::vec3(glm::lerp(begin->second.getScale(),end->second.getScale(),factor));

        //Build a matrix transformation from the orientation, translation and scale components
        return GeometricTransformation( translation, orientation, scale ).toMatrix();
    } else {
        std::map< float, GeometricTransformation >::const_iterator wanderer = std::next(begin);
        //Return the mean of the two matrices created by sub bezier curves
        glm::mat4 test = glm::mat4(1.0);
        return computeBezier(time, begin, wanderer) + computeBezier(time, wanderer, end)  * glm::mat4(0.5);
    }
}

glm::mat4 BezierKeyframeCollection::interpolateTransformation( float time ) const
{
    if(!m_keyframes.empty()) {
        //Handle the case where the time parameter is outside the keyframes time scope.
        std::map< float, GeometricTransformation >::const_iterator itFirstFrame = m_keyframes.begin();
        std::map< float, GeometricTransformation >::const_reverse_iterator itLastFrame = m_keyframes.rbegin();
        if( time <= itFirstFrame->first ) return itFirstFrame->second.toMatrix();
        if( time >= itLastFrame->first ) return itLastFrame->second.toMatrix();

        //Get first frame time-wise (if the user has been kind enough to give a coherent value)
        std::map< float, GeometricTransformation >::const_iterator begin = m_keyframes.begin();
        //Get last frame
        std::map< float, GeometricTransformation >::const_iterator end = m_keyframes.end();

        //Case where only one keyframe has been set
        if(begin == end) {  return (end->second).toMatrix();    }
        
        //Build a matrix transformation from the orientation, translation and scale components
        return computeBezier(time, begin, end);
        
    } else {   return glm::mat4(1.0);  }
}

bool BezierKeyframeCollection::empty() const
{
    return m_keyframes.empty();
}

std::array< BezierKeyframeCollection::Keyframe, 2 > BezierKeyframeCollection::getBoundingKeyframes( float time ) const
{
    std::array< BezierKeyframeCollection::Keyframe, 2 > result{ std::make_pair(0, GeometricTransformation()), std::make_pair(0, GeometricTransformation()) };
    std::map< float, GeometricTransformation >::const_iterator upper = m_keyframes.upper_bound(time);
    std::map< float, GeometricTransformation >::const_iterator lower = std::prev(upper);
    std::map< float, GeometricTransformation >::const_iterator end = m_keyframes.end();
    if(upper != end && lower != end )
    {
        result[0] = *lower;
        result[1] = *upper;
    }
    return result;
}
