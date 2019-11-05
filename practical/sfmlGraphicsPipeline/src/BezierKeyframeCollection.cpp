# include "./../include/BezierKeyframeCollection.hpp"
# include <glm/gtx/compatibility.hpp>

void BezierKeyframeCollection::add( const GeometricTransformation& transformation, float time )
{
    m_keyframes.insert( std::make_pair(time, transformation) );
}

GeometricTransformation BezierKeyframeCollection::computeBezier ( float time,
    std::map< float, GeometricTransformation >::const_iterator & begin,
    std::map< float, GeometricTransformation >::const_iterator & end ) const {

    float factor = (time - begin->first)/(end->first - begin->first);

    //If given keyframes are the last adjacent two, return a linear interpolation
    if(std::next(begin) == end ){

        
        //Interpolate each transformation component of the surrounding keyframes: orientation, translation, scale
        glm::vec3 translation = glm::vec3(glm::lerp(begin->second.getTranslation(),end->second.getTranslation(),factor));
        glm::quat orientation = glm::quat(glm::slerp(begin->second.getOrientation(),end->second.getOrientation(),factor));
        glm::vec3 scale = glm::vec3(glm::lerp(begin->second.getScale(),end->second.getScale(),factor));

        //Build a matrix transformation from the orientation, translation and scale components
        return GeometricTransformation( translation, orientation, scale );
    } else {
        std::map< float, GeometricTransformation >::const_iterator wanderer = std::next(begin);
        //Return the mean of the two matrices created by sub bezier curves
        // glm::mat4 res = (computeBezier(time, begin, wanderer) + computeBezier(time, wanderer, end)) * glm::mat4(.25f);
        GeometricTransformation g_next = computeBezier(time, begin, wanderer);
        GeometricTransformation g_rest = computeBezier(time, wanderer, end);
        
        //Interpolate each transformation component of the surrounding keyframes: orientation, translation, scale
        glm::vec3 translation = glm::vec3(glm::lerp(g_next.getTranslation(),g_rest.getTranslation(),factor));
        glm::quat orientation = glm::quat(glm::slerp(g_next.getOrientation(),g_rest.getOrientation(),factor));
        glm::vec3 scale = glm::vec3(glm::lerp(g_next.getScale(),g_rest.getScale(),factor));

        //Build a matrix transformation from the orientation, translation and scale components
        return GeometricTransformation( translation, orientation, scale );

/*         if(factor < 0.0f || factor > 1.0f ) { factor = 0.5f; }

        glm::quat q1 = glm::quat(glm::lerp(
            glm::quat {mat1[0][0], mat1[1][0], mat1[2][0], mat1[3][0]}, 
            glm::quat {mat2[0][0], mat2[1][0], mat2[2][0], mat2[3][0]},
            factor));
        glm::quat q2 = glm::quat(glm::lerp(
            glm::quat {mat1[0][1], mat1[1][1], mat1[2][1], mat1[3][1]}, 
            glm::quat {mat2[0][1], mat2[1][1], mat2[2][1], mat2[3][1]},
            factor));
        glm::quat q3 = glm::quat(glm::lerp(
            glm::quat {mat1[0][2], mat1[1][2], mat1[2][2], mat1[3][2]}, 
            glm::quat {mat2[0][2], mat2[1][2], mat2[2][2], mat2[3][2]},
            factor));
        glm::quat q4 = glm::quat(glm::lerp(
            glm::quat {mat1[0][3], mat1[1][3], mat1[2][3], mat1[3][3]}, 
            glm::quat {mat2[0][3], mat2[1][3], mat2[2][3], mat2[3][3]},
            factor));

        glm::mat4 res = glm::mat4(
            q1[0], q1[1], q1[2], q1[3],
            q2[0], q2[1], q2[2], q2[3],
            q3[0], q3[1], q3[2], q3[3],
            q4[0], q4[1], q4[2], q4[3]
        ); */
        // return res; 
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
        return computeBezier(time, begin, end).toMatrix();
        
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
