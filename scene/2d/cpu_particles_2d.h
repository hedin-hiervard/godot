#ifndef CPU_PARTICLES_2D_H
#define CPU_PARTICLES_2D_H
#include "rid.h"
#include "scene/2d/node_2d.h"
#include "scene/main/timer.h"
#include "scene/resources/material.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/

class CPUParticles2D : public Node2D {
private:
	GDCLASS(CPUParticles2D, Node2D);

public:
	enum DrawOrder {
		DRAW_ORDER_INDEX,
		DRAW_ORDER_LIFETIME,
	};

	enum Parameter {

		PARAM_INITIAL_LINEAR_VELOCITY,
		PARAM_ANGULAR_VELOCITY,
		PARAM_ORBIT_VELOCITY,
		PARAM_LINEAR_ACCEL,
		PARAM_RADIAL_ACCEL,
		PARAM_TANGENTIAL_ACCEL,
		PARAM_DAMPING,
		PARAM_ANGLE,
		PARAM_SCALE,
		PARAM_HUE_VARIATION,
		PARAM_ANIM_SPEED,
		PARAM_ANIM_OFFSET,
		PARAM_MAX
	};

	enum Flags {
		FLAG_ALIGN_Y_TO_VELOCITY,
		FLAG_ANIM_LOOP,
		FLAG_MAX
	};

	enum EmissionShape {
		EMISSION_SHAPE_POINT,
		EMISSION_SHAPE_CIRCLE,
		EMISSION_SHAPE_RECTANGLE,
		EMISSION_SHAPE_POINTS,
		EMISSION_SHAPE_DIRECTED_POINTS,
	};

private:
	bool emitting;

	struct Particle {
		Transform2D transform;
		Color color;
		float custom[4];
		Vector2 velocity;
		bool active;
		float angle_rand;
		float scale_rand;
		float hue_rot_rand;
		float anim_offset_rand;
		float time;
		Color base_color;

		uint32_t seed;
	};

	float time;
	float inactive_time;
	float frame_remainder;
	int cycle;

	RID mesh;
	RID multimesh;

	PoolVector<Particle> particles;
	PoolVector<float> particle_data;
	PoolVector<int> particle_order;

	struct SortLifetime {
		const Particle *particles;

		bool operator()(int p_a, int p_b) const {
			return particles[p_a].time < particles[p_b].time;
		}
	};

	struct SortAxis {
		const Particle *particles;
		Vector2 axis;
		bool operator()(int p_a, int p_b) const {

			return axis.dot(particles[p_a].transform[2]) < axis.dot(particles[p_b].transform[2]);
		}
	};

	//

	bool one_shot;

	float lifetime;
	float pre_process_time;
	float explosiveness_ratio;
	float randomness_ratio;
	float speed_scale;
	bool local_coords;
	int fixed_fps;
	bool fractional_delta;

	DrawOrder draw_order;

	Ref<Texture> texture;
	Ref<Texture> normalmap;

	////////

	float spread;
	float flatness;

	float parameters[PARAM_MAX];
	float randomness[PARAM_MAX];

	Ref<Curve> curve_parameters[PARAM_MAX];
	Color color;
	Ref<Gradient> color_ramp;

	bool flags[FLAG_MAX];

	EmissionShape emission_shape;
	float emission_sphere_radius;
	Vector2 emission_rect_extents;
	PoolVector<Vector2> emission_points;
	PoolVector<Vector2> emission_normals;
	PoolVector<Color> emission_colors;
	int emission_point_count;

	bool anim_loop;
	Vector2 gravity;

	void _particles_process(float p_delta);
	void _update_particle_data_buffer();

	Mutex *update_mutex;

	void _update_render_thread();

	void _update_mesh_texture();

protected:
	static void _bind_methods();
	void _notification(int p_what);
	virtual void _validate_property(PropertyInfo &property) const;

public:
	void set_emitting(bool p_emitting);
	void set_amount(int p_amount);
	void set_lifetime(float p_lifetime);
	void set_one_shot(bool p_one_shot);
	void set_pre_process_time(float p_time);
	void set_explosiveness_ratio(float p_ratio);
	void set_randomness_ratio(float p_ratio);
	void set_visibility_aabb(const Rect2 &p_aabb);
	void set_use_local_coordinates(bool p_enable);
	void set_speed_scale(float p_scale);

	bool is_emitting() const;
	int get_amount() const;
	float get_lifetime() const;
	bool get_one_shot() const;
	float get_pre_process_time() const;
	float get_explosiveness_ratio() const;
	float get_randomness_ratio() const;
	Rect2 get_visibility_aabb() const;
	bool get_use_local_coordinates() const;
	float get_speed_scale() const;

	void set_fixed_fps(int p_count);
	int get_fixed_fps() const;

	void set_fractional_delta(bool p_enable);
	bool get_fractional_delta() const;

	void set_draw_order(DrawOrder p_order);
	DrawOrder get_draw_order() const;

	void set_draw_passes(int p_count);
	int get_draw_passes() const;

	void set_texture(const Ref<Texture> &p_texture);
	Ref<Texture> get_texture() const;

	void set_normalmap(const Ref<Texture> &p_normalmap);
	Ref<Texture> get_normalmap() const;

	///////////////////

	void set_spread(float p_spread);
	float get_spread() const;

	void set_flatness(float p_flatness);
	float get_flatness() const;

	void set_param(Parameter p_param, float p_value);
	float get_param(Parameter p_param) const;

	void set_param_randomness(Parameter p_param, float p_value);
	float get_param_randomness(Parameter p_param) const;

	void set_param_curve(Parameter p_param, const Ref<Curve> &p_curve);
	Ref<Curve> get_param_curve(Parameter p_param) const;

	void set_color(const Color &p_color);
	Color get_color() const;

	void set_color_ramp(const Ref<Gradient> &p_texture);
	Ref<Gradient> get_color_ramp() const;

	void set_particle_flag(Flags p_flag, bool p_enable);
	bool get_particle_flag(Flags p_flag) const;

	void set_emission_shape(EmissionShape p_shape);
	void set_emission_sphere_radius(float p_radius);
	void set_emission_rect_extents(Vector2 p_extents);
	void set_emission_points(const PoolVector<Vector2> &p_points);
	void set_emission_normals(const PoolVector<Vector2> &p_normals);
	void set_emission_colors(const PoolVector<Color> &p_colors);
	void set_emission_point_count(int p_count);

	EmissionShape get_emission_shape() const;
	float get_emission_sphere_radius() const;
	Vector2 get_emission_rect_extents() const;
	PoolVector<Vector2> get_emission_points() const;
	PoolVector<Vector2> get_emission_normals() const;
	PoolVector<Color> get_emission_colors() const;
	int get_emission_point_count() const;

	void set_gravity(const Vector2 &p_gravity);
	Vector2 get_gravity() const;

	virtual String get_configuration_warning() const;

	void restart();

	void convert_from_particles(Node *p_particles);

	CPUParticles2D();
	~CPUParticles2D();
};

VARIANT_ENUM_CAST(CPUParticles2D::DrawOrder)
VARIANT_ENUM_CAST(CPUParticles2D::Parameter)
VARIANT_ENUM_CAST(CPUParticles2D::Flags)
VARIANT_ENUM_CAST(CPUParticles2D::EmissionShape)

#endif // CPU_PARTICLES_2D_H