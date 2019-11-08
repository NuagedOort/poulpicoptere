# include "./../include/BezierKeyframeCollection.hpp"
# include <glm/gtx/compatibility.hpp>
# include <iostream>
void BezierKeyframeCollection::add( const GeometricTransformation& transformation, float time )
{
    m_keyframes.insert( std::make_pair(time, transformation) );
}

GeometricTransformation BezierKeyframeCollection::computeBezier ( const float time_current, const float time_start, const float time_end,
    std::map< float, GeometricTransformation >::const_iterator & begin,
    std::map< float, GeometricTransformation >::const_reverse_iterator & end ) const {

    float factor = (time_current - time_start) / time_end;
    std::cout <<"****TIME FACTOR****\nTIME : "<<time_current<<"\nBEGIN : "<<begin->first<<"\nEND : "<<end->first<<"\nFACTOR : "<<factor<<"\n\n";

    //If given keyframes are the last adjacent two, return a linear interpolation
    if(std::next(begin)->first == end->first ){
        
        //Interpolate each transformation component of the surrounding keyframes: orientation, translation, scale
        glm::vec3 translation = glm::vec3(glm::lerp(begin->second.getTranslation(),end->second.getTranslation(),factor));
        glm::quat orientation = glm::quat(glm::slerp(begin->second.getOrientation(),end->second.getOrientation(),factor));
        glm::vec3 scale = glm::vec3(glm::lerp(begin->second.getScale(),end->second.getScale(),factor));

        std::cout << "Translation : " << translation[0] << ",\t " << translation[1] << ",\t " << translation[2] << "\n"; 

        //Build a matrix transformation from the orientation, translation and scale components
        return GeometricTransformation( translation, orientation, scale );
    } else {
        std::map< float, GeometricTransformation >::const_iterator wanderer = std::next(begin);
        std::map< float, GeometricTransformation >::const_reverse_iterator backWanderer = end;
        //Haven't been able to revert the forward iterator to reverse iteraotr unfortunately
        while(backWanderer->first > wanderer->first) {
            backWanderer++;
        }

        //Return the mean of the two matrices created by sub bezier curves
        GeometricTransformation g_next = computeBezier(time_current, time_start, time_end, begin, backWanderer);
        GeometricTransformation g_rest = computeBezier(time_current, time_start, time_end, wanderer, end);
        
        //Interpolate each transformation component of the surrounding keyframes: orientation, translation, scale
        glm::vec3 translation = glm::vec3(glm::lerp(g_next.getTranslation(),g_rest.getTranslation(),factor));
        glm::quat orientation = glm::quat(glm::slerp(g_next.getOrientation(),g_rest.getOrientation(),factor));
        glm::vec3 scale = glm::vec3(glm::lerp(g_next.getScale(),g_rest.getScale(),factor));

        std::cout << "Translation : " << translation[0] << ",\t " << translation[1] << ",\t " << translation[2] << "\n";

        //Build a matrix transformation from the orientation, translation and scale components
        return GeometricTransformation( translation, orientation, scale ); 
    }
}

glm::mat4 BezierKeyframeCollection::interpolateTransformation( float time_current, float time_start, float time_end ) const
{
    if(!m_keyframes.empty()) {
        //Handle the case where the time parameter is outside the keyframes time scope.
        std::map< float, GeometricTransformation >::const_iterator itFirstFrame = m_keyframes.begin();
        std::map< float, GeometricTransformation >::const_reverse_iterator itLastFrame = m_keyframes.rbegin();

        if( time_current <= itFirstFrame->first ) return itFirstFrame->second.toMatrix();
        if( time_current >= itLastFrame->first ) return itLastFrame->second.toMatrix();

        std::map< float, GeometricTransformation >::const_iterator itForward = m_keyframes.begin();
        std::map< float, GeometricTransformation >::const_reverse_iterator itBackward = m_keyframes.rbegin();

        for(itForward; (itForward->first < time_start) && (itForward->first < itLastFrame->first); itForward++) {
            itFirstFrame++;
        }
        for(itBackward; (itBackward->first > time_end) && (itBackward->first > itFirstFrame->first); itBackward++) {
            itLastFrame++;
        }
        
        std::cout << "TIME : " << time_current << "\n";
        std::cout << "BEGIN INIT : " << itFirstFrame->first << "\n";
        std::cout << "END INIT : " << itLastFrame->first << "\n\n";
        
        //Case where only one keyframe has been set
        if(itFirstFrame->first == itLastFrame->first) {  return (itLastFrame->second).toMatrix();    }
        
        //Build a matrix transformation from the orientation, translation and scale components
        return computeBezier(time_current, time_start, time_end, itFirstFrame, itLastFrame).toMatrix();
        
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
