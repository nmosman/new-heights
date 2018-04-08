//==============================================================================
/*
    CPSC 599.86 / 601.86 - Computer Haptics
    Winter 2018, University of Calgary

    This class extends the cAlgorithmFingerProxy class in CHAI3D that
    implements the god-object/finger-proxy haptic rendering algorithm.
    It allows us to modify or recompute the force that is ultimately sent
    to the haptic device.

    Your job for this assignment is to implement the updateForce() method
    in this class to support for two new effects: force shading and haptic
    textures. Methods for both are described in Ho et al. 1999.
*/
//==============================================================================

#include "MyProxyAlgorithm.h"
#include "MyMaterial.h"

using namespace chai3d;

//==============================================================================
/*!
    This method uses the information computed earlier in
    computeNextBestProxyPosition() to calculate the force to be rendered.
    It first calls cAlgorithmFingerProxy::updateForce() to compute the base
    force from contact geometry and the constrained proxy position. That
    force can then be modified or recomputed in this function.

    Your implementation of haptic texture mapping will likely end up in this
    function. When this function is called, collision detection has already
    been performed, and the proxy point has already been updated based on the
    constraints found. Your job is to compute a force with all that information
    available to you.

    Useful variables to read:
        m_deviceGlobalPos   - current position of haptic device
        m_proxyGlboalPos    - computed position of the constrained proxy
        m_numCollisionEvents- the number of surfaces constraining the proxy
        m_collisionRecorderConstraint0,1,2
                            - up to three cCollisionRecorder structures with
                              cCollisionEvents that contain very useful
                              information about each contact

    Variables that this function should set/reset:
        m_normalForce       - computed force applied in the normal direction
        m_tangentialForce   - computed force along the tangent of the surface
        m_lastGlobalForce   - this is what the operator ultimately feels!!!
*/
//==============================================================================

void MyProxyAlgorithm::updateForce()
{
    // get the base class to do basic force computation first
    cAlgorithmFingerProxy::updateForce();

    // TODO: compute force shading and texture forces here

	m_debugInteger = m_numCollisionEvents;
    if (m_numCollisionEvents > 0)
    {
        // this is how you access collision information from the first constraint
        cCollisionEvent* c0 = &m_collisionRecorderConstraint0.m_nearestCollision;
		cVector3d perturbed_norm = cAlgorithmFingerProxy::computeShadedSurfaceNormal(c0);
	
		//m_debugVector = c0->m_globalNormal;
		int triangleIndex = c0->m_index;
		int vertexIndex0 = c0->m_triangles->getVertexIndex0(triangleIndex);

		//m_debugVector = c0->m_triangles->m_vertices->getTangent(vertexIndex0);

        if (MyMaterialPtr material = std::dynamic_pointer_cast<MyMaterial>(c0->m_object->m_material))
        {
			
			//m_debugVector = c0->m_object->getGlobalPos();
			cVector3d texCoords = c0->m_triangles->getTexCoordAtPosition(c0->m_index, c0->m_localPos);
			m_debugVector = texCoords;
			// Hacky way of determining collision within the bumps
			if (material->name == "bumps.png")
			{
				double bump_freq = 10.0;
				double n_dhdx = cos(M_PI * 2 * bump_freq * texCoords.x());
				cVector3d surfaceNorm = cNormalize(perturbed_norm + n_dhdx*cNormalize(m_proxyGlobalPos));
				m_lastGlobalForce = m_lastGlobalForce.length() * surfaceNorm;
			}

			else if (material->name == "Leather_padded_001_colour.jpg")
			{
				chai3d::cColorb pix_color;
				cTexture2d* nMap = material->normalMap.get();
				double pixel_x = 0.0;
				double pixel_y = 0.0;
				nMap->m_image.get()->getPixelLocationInterpolated(texCoords, pixel_x, pixel_y);
				nMap->m_image.get()->getPixelColorInterpolated(pixel_x, pixel_y, pix_color);

				cVector3d h_vect = cVector3d(pix_color.getG(), pix_color.getR(), pix_color.getB());
				double h = h_vect.length() / 255.0   * material->m_materialScale;
				cVector3d map_normal = cNormalize(cVector3d(pix_color.getG() , pix_color.getR(), pix_color.getB()));
			
				cVector3d avg_norm = (map_normal + perturbed_norm) / 2.0;
				
				
				m_lastGlobalForce = m_lastGlobalForce.length() * (1.0 + h_vect.length()/255.0) * avg_norm ;
			}
			else if (material->name == "friction.jpg")
			{
				double friction_freq = 5.0;
				double n_dhdx = pow(sin(M_PI * 2 * friction_freq * texCoords.y()), 3);

				if (n_dhdx <  0)
				{
					material->setDynamicFriction(0.0);
					material->setStaticFriction(0.0);
				}

				else
				{
					material->setDynamicFriction(0.8);
					material->setStaticFriction(1.0);
				}

				
			}

			else
			{
				cVector3d texCoords = c0->m_triangles->getTexCoordAtPosition(c0->m_index, c0->m_localPos);
				cVector3d surface_norm = cAlgorithmFingerProxy::computeShadedSurfaceNormal(c0);
				// Clamp texture coordinates 
				if (texCoords.x() > 1.0)
				{
					texCoords.set(texCoords.x() - 1.0, texCoords.y(), texCoords.z());
				}
				else if (texCoords.x() < 0.0)
				{
					texCoords.set(1.0 + texCoords.x(), texCoords.y(), texCoords.z());
				}
				else if (texCoords.y() > 1.0)
				{
					texCoords.set(texCoords.x(), texCoords.y() - 1.0, texCoords.z());
				}
				else if (texCoords.y() < 0.0)
				{
					texCoords.set(texCoords.x(), 1.0 + texCoords.y(), texCoords.z());
				}
				

				chai3d::cColorb pix_color;
				cTexture2d* nMap = material->normalMap.get();
				double pixel_x = 0.0;
				double pixel_y = 0.0;
				nMap->m_image.get()->getPixelLocationInterpolated(texCoords, pixel_x, pixel_y);
				nMap->m_image.get()->getPixelColorInterpolated(pixel_x, pixel_y, pix_color);

				//R is y, G is x, B is z
				cVector3d map_normal = cNormalize(cVector3d(pix_color.getG() - 127, pix_color.getR() - 127, pix_color.getB() - 127));
				
				double k = 900.0;


				// calculation of phi and theta (spherical coordinates, thanks Electromagnetics!)
				double phi = acos(cDot(surface_norm, cVector3d(0.0, 0.0, 1.0))/surface_norm.length());
				
				//then project surface_norm to the xy plane and then calculate theta with respect to the y-axis
				cVector3d norm_xy = surface_norm - (cDot(surface_norm, cVector3d(0.0, 0.0, 1.0))*cVector3d(0.0, 0.0, 1.0));
				double theta = acos(cDot(norm_xy, cVector3d(0.0, 1.0, 0.0)) / norm_xy.length());

				if (phi != 0 && theta != 0)
				{
					double z = map_normal.z() * cos(phi) - map_normal.x() * sin(phi);
					double x = map_normal.z() * sin(phi) - map_normal.x() * cos(phi);
					double y = map_normal.y();
					map_normal.set(x, y, z);

					//now carry out rotation wrt z axis

					z = map_normal.z();
					x = map_normal.x() * cos(theta) - map_normal.y() * sin(theta);
					y = map_normal.x() * sin(theta) - map_normal.y() * cos(theta);
					map_normal.set(x, y, z);

				}
				//carry out rotation wrt y axis first
			
				//map_normal = (surface_norm + map_normal ) / 2.0;
				cTexture2d* hMap = material->heightMap.get();
			    pixel_x = 0.0;
				pixel_y = 0.0;
				hMap->m_image.get()->getPixelLocationInterpolated(texCoords, pixel_x, pixel_y);
				hMap->m_image.get()->getPixelColorInterpolated(pixel_x, pixel_y, pix_color);
				cVector3d h_vect = cVector3d(pix_color.getG(), pix_color.getR(), pix_color.getB());
			
				//double h = pix_color.getR() - 127;
				//normalize height and multiply by "material" height factor
				double h = pix_color.getR() / 255.0   * material->m_materialScale;

				
				double depth = ((m_proxyGlobalPos - m_deviceGlobalPos) + (surface_norm * h)).length() - material->m_materialScale;
				m_debugVector = cVector3d(phi, depth, h);

				map_normal.set(map_normal.x(), map_normal.y(), abs(map_normal.z()));
			
				cVector3d M = surface_norm - map_normal + (cDot(map_normal, surface_norm) * surface_norm);
				cVector3d force;
				double K = 1.0;

				if (depth >= K * h)
				{
					force = (depth - h) * perturbed_norm + (K*h*M);
					force *= k;
				}

				else
				{
					force = k*depth * M;
				}


				// get roughness params

				cTexture2d* rMap = material->roughMap.get();
				pixel_x = 0.0;
				pixel_y = 0.0;
				rMap->m_image.get()->getPixelLocationInterpolated(texCoords, pixel_x, pixel_y);
				rMap->m_image.get()->getPixelColorInterpolated(pixel_x, pixel_y, pix_color);

				double kinetic_friction = pix_color.getR() / 255.0 * material->m_materialFriction;
				double static_friction = 1.25 * kinetic_friction;
				
				material->setDynamicFriction(kinetic_friction);
				material->setStaticFriction(static_friction);


				m_debugNMap = map_normal;
				m_debugPerturbedNormal = M;
				m_debugForce = force;
				force.set(force.x(), force.y(),abs(force.z()));
				m_lastGlobalForce = force;
			}
			/* Pt.3 STUFF 
				//
				cVector3d texCoords = c0->m_triangles->getTexCoordAtPosition(c0->m_index, c0->m_localPos); 
				m_debugVector = texCoords;
				double bump_freq = 10.0;
				double h = sin(M_PI * 2 * bump_freq * texCoords.x());

				double dhdx = M_PI * 2 * bump_freq*cos(M_PI * 2 * bump_freq * texCoords.y());
				double depth = cDistance(m_proxyGlobalPos + h*cVector3d(0.0, 0.0, 1.0), m_deviceGlobalPos);
				cVector3d h_grad(0, dhdx, 0.0);

				cVector3d M = perturbed_norm - h_grad + (cDot(h_grad, perturbed_norm) * perturbed_norm);
				cVector3d force;
				double K = 1.0;
				if (depth >= K*h)
				{
					force = (depth - h) * perturbed_norm + (K*h*M);
				}

				else
				{
					force = depth * M;
				}

				m_lastGlobalForce = force;
			*/
            // you can access your custom material properties here
        }

    }
}


//==============================================================================
/*!
    This method attempts to move the proxy, subject to friction constraints.
    This is called from computeNextBestProxyPosition() when the proxy is
    ready to move along a known surface.

    Your implementation of friction mapping will likely need to modify or
    replace the CHAI3D implementation in cAlgorithmFingerProxy. You may
    either copy the implementation from the base class and modify it to take
    into account a friction map, or use your own friction rendering from your
    previous assignment.

    The most important thing to do in this method is to write the desired
    proxy position into the m_nextBestProxyGlobalPos member variable.

    The input parameters to this function are as follows, all provided in the
    world (global) coordinate frame:

    \param  a_goal    The location to which we'd like to move the proxy.
    \param  a_proxy   The current position of the proxy.
    \param  a_normal  The surface normal at the obstructing surface.
    \param  a_parent  The surface along which we're moving.
*/
//==============================================================================
void MyProxyAlgorithm::testFrictionAndMoveProxy(const cVector3d& a_goal,
                                                const cVector3d& a_proxy,
                                                cVector3d &a_normal,
                                                cGenericObject* a_parent)
{
    cAlgorithmFingerProxy::testFrictionAndMoveProxy(a_goal, a_proxy, a_normal, a_parent);
}
