#include "ColorTextureProgram.hpp"

#include "Mode.hpp"
#include "GL.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

/*
 * FoosballMode is a game mode that implements a single-player game of Foosball.
 */

struct FoosballMode : Mode {
	FoosballMode();
	virtual ~FoosballMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----

	glm::vec2 court_radius = glm::vec2(15.0f, 12.0f);
	glm::vec2 paddle_radius = glm::vec2(0.3f, 0.5f);
	glm::vec2 ball_radius = glm::vec2(0.2f, 0.2f);

    std::vector<glm::vec2> left_defenders{glm::vec2(-2*court_radius.x/3, court_radius.y/4), glm::vec2(-2*court_radius.x/3, 0), glm::vec2(-2*court_radius.x/3, -court_radius.y/4)};
    std::vector<glm::vec2> left_strikers{glm::vec2(court_radius.x/5, court_radius.y/4), glm::vec2(court_radius.x/5, -court_radius.y/4)};
    std::vector<glm::vec2> right_defenders{glm::vec2(2*court_radius.x/3, court_radius.y/4), glm::vec2(2*court_radius.x/3, 0), glm::vec2(2*court_radius.x/3, -court_radius.y/4)};
    std::vector<glm::vec2> right_strikers{glm::vec2(-court_radius.x/5, court_radius.y/4), glm::vec2(-court_radius.x/5, -court_radius.y/4)};

	glm::vec2 ball = glm::vec2(-2*court_radius.x/3+ball_radius.x, 0.0f);
    float velocity = 3.0f;
    float net_radius = 2.0f;
	glm::vec2 ball_velocity = glm::vec2(0.0f, 0.0f);

	uint32_t left_score = 0;
	uint32_t right_score = 0;
	float celebration = 0.0f;

	float ai_offset = 0.0f;
	float ai_offset_update = 0.0f;

	bool w_pressed = false;
	bool s_pressed = false;
	int space_pressed = 0;
    bool shift_pressed = false;
    bool q_pressed = true;
    bool e_pressed = true;
    bool up_pressed = false;
    bool down_pressed = false;
    bool return_pressed = false;
    bool autos_pressed = false;
    bool autod_pressed = true;

    bool unblock_right_strikers = false;
    bool unblock_right_defenders = false;

    float speed_multiplier = 5;

    int scored = 0;

	//----- pretty rainbow trails -----

//	float trail_length = 1.3f;
//	std::deque< glm::vec3 > ball_trail; //stores (x,y,age), oldest elements first

	//----- opengl assets / helpers ------

	//draw functions will work on vectors of vertices, defined as follows:
	struct Vertex {
		Vertex(glm::vec3 const &Position_, glm::u8vec4 const &Color_, glm::vec2 const &TexCoord_) :
			Position(Position_), Color(Color_), TexCoord(TexCoord_) { }
		glm::vec3 Position;
		glm::u8vec4 Color;
		glm::vec2 TexCoord;
	};
	static_assert(sizeof(Vertex) == 4*3 + 1*4 + 4*2, "FoosballMode::Vertex should be packed");

	//Shader program that draws transformed, vertices tinted with vertex colors:
	ColorTextureProgram color_texture_program;

	//Buffer used to hold vertex data during drawing:
	GLuint vertex_buffer = 0;

	//Vertex Array Object that maps buffer locations to color_texture_program attribute locations:
	GLuint vertex_buffer_for_color_texture_program = 0;

	//Solid white texture:
	GLuint white_tex = 0;

	//matrix that maps from clip coordinates to court-space coordinates:
	glm::mat3x2 clip_to_court = glm::mat3x2(1.0f);
	// computed in draw() as the inverse of OBJECT_TO_CLIP
	// (stored here so that the mouse handling code can use it to position the paddle)

};
