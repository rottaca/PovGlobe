#include "renderer_sim.hpp"
#include "globe.hpp"
#include <iostream>

using namespace cimg_library;

RendererSim::RendererSim(
	RpmMeasureBase& rpm_measure_base)
	:RendererBase(rpm_measure_base)
{
}

RendererSim::~RendererSim()
{
}

constexpr double pi = 3.14159265358979323846;

cimg_library::CImg<float> buildSphere(cimg_library::CImgList<unsigned int> & primitives, 
	int n_segments_xy, int n_segments_z, 
	float radius = 1.0f, 
	float spacing_top = 0.0f, float spacing_bottom = 0.0f) {

	CImgList<float> vertices;

	primitives.assign();
	vertices.assign();

	std::vector<float> sin_z(n_segments_z+1);
	std::vector<float> cos_z(n_segments_z+1);

	float min_angle_z = spacing_top / radius;
	float max_angle_z = pi - spacing_bottom / radius;

	auto scaled_angle_xy = [n_segments_xy, min = -pi, max = pi](float xy) {return (xy / n_segments_xy) * (max - min) + min; };
	auto scaled_angle_z = [n_segments_z, min = min_angle_z, max = max_angle_z](float z) {return (z / n_segments_z) * (max - min) + min; };

	for(size_t z = 0; z < n_segments_z + 1; z++) {
		float angle_z = scaled_angle_z(static_cast<float>(z));
		sin_z[z] = std::sin(angle_z);
		cos_z[z] = std::cos(angle_z);		
	}

	for(size_t xy = 0; xy < n_segments_xy; xy++) {
		float prev_angle_xy = scaled_angle_xy(static_cast<float>(xy) - 0.5f);
		float prev_cos_xy = std::cos(prev_angle_xy);
		float prev_sin_xy = std::sin(prev_angle_xy);

		float next_angle_xy = scaled_angle_xy(static_cast<float>(xy) + 0.5f);
		float next_cos_xy = std::cos(next_angle_xy);
		float next_sin_xy = std::sin(next_angle_xy);

		int vertex_start_idx = vertices.size();

		for(size_t z = 0; z < n_segments_z + 1; z++) {
			float prev_pos_x = radius * prev_cos_xy * sin_z[z];
			float prev_pos_y = radius * prev_sin_xy * sin_z[z];
			float next_pos_x = radius * next_cos_xy * sin_z[z];
			float next_pos_y = radius * next_sin_xy * sin_z[z];
			float pos_z = radius * cos_z[z];

			CImg<float>::vector(prev_pos_x, prev_pos_y, pos_z).move_to(vertices);
			CImg<float>::vector(next_pos_x, next_pos_y, pos_z).move_to(vertices);
		}
		
		for(size_t z = 0; z < n_segments_z; z++) {
			int p0 = vertex_start_idx + z*2 + 2;
			int p1 = vertex_start_idx + z * 2 + 3;
			int p2 = vertex_start_idx + z * 2 + 1;
			int p3 = vertex_start_idx + z * 2 + 0;
			CImg<unsigned int>::vector(p0, p1, p2, p3).move_to(primitives);
		}
	}

	return  (vertices > 'x');
}

void RendererSim::initialize(Globe& globe)
{
	RendererBase::initialize(globe);
	m_draw_display.assign(3 * globe.getWidth(), 3 * globe.getHeight(), "Flat Visualization");
	m_draw_display.move(0, 0);

	m_draw_display_3d.assign(512, 512, "3D Visualization");
	m_draw_display_3d.move(3 * globe.getWidth(), 0);

	m_sphere_pts = buildSphere(m_sphere_primitives, 
							globe.getWidth(), globe.getHeight(), 
							globe.getRadius(), 
							globe.getSpacingTop(), globe.getSpacingBottom());
	// Rotate to show user the right side of the globe
	m_sphere_pts = CImg<>::rotation_matrix(0, 0, 1, -90) * CImg<>::rotation_matrix(0, 1, 0, 90) * m_sphere_pts;
	m_sphere_colors = CImgList<unsigned char>(m_sphere_primitives.size(), CImg<unsigned char>::vector(0, 0, 0));
	m_sphere_opacs = CImg<float>(m_sphere_primitives.size(), 1, 1, 1, 0.7f);

	m_visu_buffer3d = CImg<unsigned char>(3, m_draw_display_3d.height(), m_draw_display_3d.width(), 1).permute_axes("yzcx");
	m_visu_buffer = CImg<unsigned char>(globe.getWidth(), globe.getHeight(), 1, 3);
}

void RendererSim::render(const Framebuffer& buffer)
{
	if (!m_draw_display.is_closed()) {
		for(size_t i = 0; i < buffer.getHeight(); i++)
		{
			for(size_t j = 0; j < buffer.getWidth(); j++)
			{
				for(size_t c = 0; c < buffer.getChannels(); c++)
				{
					m_visu_buffer(j, i, 0, c) = buffer(i, j, c);
				}
			}
		}
		m_draw_display.display(m_visu_buffer.get_resize_tripleXY());
	}

	if (!m_draw_display_3d.is_closed()) {
		for(size_t j = 0; j < buffer.getWidth(); j++)
		{
			for(size_t i = 0; i < buffer.getHeight(); i++)
			{
				m_sphere_colors[j*buffer.getHeight() + i] = CImg<unsigned char>::vector(
					buffer(i, j, 0),
					buffer(i, j, 1),
					buffer(i, j, 2));
			}

			m_visu_buffer3d.fill(255);
			m_visu_buffer3d.draw_object3d(255, 255, 255,
				m_sphere_pts * 20,
				m_sphere_primitives,
				m_sphere_colors,
				m_sphere_opacs,
				2, false, 300
			);

			m_draw_display_3d.display(m_visu_buffer3d);
		}
	}
}
