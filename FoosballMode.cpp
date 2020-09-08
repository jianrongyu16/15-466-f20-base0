#include "FoosballMode.hpp"

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

#include <random>

FoosballMode::FoosballMode() {

	
	//----- allocate OpenGL resources -----
	{ //vertex buffer:
		glGenBuffers(1, &vertex_buffer);
		//for now, buffer will be un-filled.

		GL_ERRORS(); //PARANOIA: print out any OpenGL errors that may have happened
	}

	{ //vertex array mapping buffer for color_texture_program:
		//ask OpenGL to fill vertex_buffer_for_color_texture_program with the name of an unused vertex array object:
		glGenVertexArrays(1, &vertex_buffer_for_color_texture_program);

		//set vertex_buffer_for_color_texture_program as the current vertex array object:
		glBindVertexArray(vertex_buffer_for_color_texture_program);

		//set vertex_buffer as the source of glVertexAttribPointer() commands:
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

		//set up the vertex array object to describe arrays of FoosballMode::Vertex:
		glVertexAttribPointer(
			color_texture_program.Position_vec4, //attribute
			3, //size
			GL_FLOAT, //type
			GL_FALSE, //normalized
			sizeof(Vertex), //stride
			(GLbyte *)0 + 0 //offset
		);
		glEnableVertexAttribArray(color_texture_program.Position_vec4);
		//[Note that it is okay to bind a vec3 input to a vec4 attribute -- the w component will be filled with 1.0 automatically]

		glVertexAttribPointer(
			color_texture_program.Color_vec4, //attribute
			4, //size
			GL_UNSIGNED_BYTE, //type
			GL_TRUE, //normalized
			sizeof(Vertex), //stride
			(GLbyte *)0 + 4*3 //offset
		);
		glEnableVertexAttribArray(color_texture_program.Color_vec4);

		glVertexAttribPointer(
			color_texture_program.TexCoord_vec2, //attribute
			2, //size
			GL_FLOAT, //type
			GL_FALSE, //normalized
			sizeof(Vertex), //stride
			(GLbyte *)0 + 4*3 + 4*1 //offset
		);
		glEnableVertexAttribArray(color_texture_program.TexCoord_vec2);

		//done referring to vertex_buffer, so unbind it:
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//done setting up vertex array object, so unbind it:
		glBindVertexArray(0);

		GL_ERRORS(); //PARANOIA: print out any OpenGL errors that may have happened
	}

	{ //solid white texture:
		//ask OpenGL to fill white_tex with the name of an unused texture object:
		glGenTextures(1, &white_tex);

		//bind that texture object as a GL_TEXTURE_2D-type texture:
		glBindTexture(GL_TEXTURE_2D, white_tex);

		//upload a 1x1 image of solid white to the texture:
		glm::uvec2 size = glm::uvec2(1,1);
		std::vector< glm::u8vec4 > data(size.x*size.y, glm::u8vec4(0xff, 0xff, 0xff, 0xff));
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

		//set filtering and wrapping parameters:
		//(it's a bit silly to mipmap a 1x1 texture, but I'm doing it because you may want to use this code to load different sizes of texture)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		//since texture uses a mipmap and we haven't uploaded one, instruct opengl to make one for us:
		glGenerateMipmap(GL_TEXTURE_2D);

		//Okay, texture uploaded, can unbind it:
		glBindTexture(GL_TEXTURE_2D, 0);

		GL_ERRORS(); //PARANOIA: print out any OpenGL errors that may have happened
	}
}

FoosballMode::~FoosballMode() {

	//----- free OpenGL resources -----
	glDeleteBuffers(1, &vertex_buffer);
	vertex_buffer = 0;

	glDeleteVertexArrays(1, &vertex_buffer_for_color_texture_program);
	vertex_buffer_for_color_texture_program = 0;

	glDeleteTextures(1, &white_tex);
	white_tex = 0;
}

bool FoosballMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
    if (evt.type == SDL_KEYUP) {
        if (evt.key.keysym.sym == SDLK_LSHIFT) {
            shift_pressed = false;
        }
        if (evt.key.keysym.sym == SDLK_w) {
            w_pressed = false;
        }
        if (evt.key.keysym.sym == SDLK_s) {
            s_pressed = false;
        }
        if (evt.key.keysym.sym == SDLK_SPACE) {
            space_pressed = 0;
        }
        if (evt.key.keysym.sym == SDLK_q) {
            q_pressed = !q_pressed;
        }
        if (evt.key.keysym.sym == SDLK_e) {
            e_pressed = !e_pressed;
        }
        if (evt.key.keysym.sym == SDLK_UP) {
            up_pressed = false;
        }
        if (evt.key.keysym.sym == SDLK_DOWN) {
            down_pressed = false;
        }
        if (evt.key.keysym.sym == SDLK_RETURN) {
            return_pressed = false;
        }
        if (evt.key.keysym.sym == SDLK_1) {
            autod_pressed = !autod_pressed;
        }
        if (evt.key.keysym.sym == SDLK_3) {
            autos_pressed = !autos_pressed;
        }
    }
    if (evt.type == SDL_KEYDOWN) {
        if (evt.key.keysym.sym == SDLK_LSHIFT) {
            shift_pressed = true;
        }
        if (evt.key.keysym.sym == SDLK_w) {
            w_pressed = true;
        }
        if (evt.key.keysym.sym == SDLK_s) {
            s_pressed = true;
        }
        if (evt.key.keysym.sym == SDLK_SPACE && space_pressed == 0) {
            space_pressed = 1;
        }
        if (evt.key.keysym.sym == SDLK_UP) {
            up_pressed = true;
        }
        if (evt.key.keysym.sym == SDLK_DOWN) {
            down_pressed = true;
        }
        if (evt.key.keysym.sym == SDLK_RETURN) {
            return_pressed = true;
        }

    }
	return false;
}

void FoosballMode::update(float elapsed) {

	static std::mt19937 mt; //mersenne twister pseudo-random number generator

    auto move_players = [this](std::vector<glm::vec2> &players, float move) {
        for (auto p: players) {
            float pos = p.y+move;
            if (pos>court_radius.y - paddle_radius.y) {
                move = court_radius.y - paddle_radius.y - p.y;
            } else if (pos<-court_radius.y + paddle_radius.y) {
                move = -court_radius.y + paddle_radius.y - p.y;
            }
        }
        for (auto &p: players) {
            p.y += move;
        }
    };

    if (celebration>0) {
        if (celebration-elapsed >0) {
            celebration-=elapsed;
            return;
        } else {
            celebration=0.0f;
            if (scored == 0) {
                ball = glm::vec2(-2*court_radius.x/3+ball_radius.x, 0.0f);
            } else {
                ball = glm::vec2(2*court_radius.x/3-ball_radius.x, 0.0f);
            }
        }
    }

	{ //right player ai:
		ai_offset_update -= elapsed;
		if (ai_offset_update < elapsed) {
			//update again in [0.5,1.0) seconds:
			ai_offset_update = (mt() / float(mt.max())) * 0.5f + 0.5f;
			ai_offset = (mt() / float(mt.max())) * 2.5f - 1.25f;
		}
		float ai_speed = 2.0f;
		float stball_y = ball_velocity.x == 0? ball.y:(right_strikers[0].x - ball.x)*ball_velocity.y/ball_velocity.x+ball.y;
        float dfball_y = ball_velocity.x == 0? ball.y:(right_defenders[0].x - ball.x)*ball_velocity.y/ball_velocity.x+ball.y;
		if (ball.x<right_strikers[0].x) {
		    if (stball_y>0) {
                if (right_strikers[0].y < stball_y) {
                    move_players(right_strikers, ai_speed * elapsed);
                } else if (right_strikers[1].y > stball_y) {
                    move_players(right_strikers, -ai_speed * elapsed);
                } else if (stball_y<right_strikers[0].y-paddle_radius.y && stball_y>right_strikers[1].y+paddle_radius.y) {
                    move_players(right_strikers, -ai_speed * elapsed);
                }
		    } else {
                if (right_strikers[0].y < stball_y) {
                    move_players(right_strikers, ai_speed * elapsed);
                } else if (right_strikers[1].y > stball_y) {
                    move_players(right_strikers, -ai_speed * elapsed);
                } else if (stball_y<right_strikers[0].y-paddle_radius.y && stball_y>right_strikers[1].y+paddle_radius.y) {
                    move_players(right_strikers, ai_speed * elapsed);
                }
		    }
            unblock_right_defenders = false;
            unblock_right_strikers = false;
		} else if (ball.x<right_defenders[0].x) {
            if (dfball_y>0) {
                if (right_defenders[0].y < dfball_y) {
                    move_players(right_defenders, ai_speed * elapsed);
                } else if (right_defenders[2].y > dfball_y) {
                    move_players(right_defenders, -ai_speed * elapsed);
                } else if (dfball_y<right_defenders[0].y-paddle_radius.y && dfball_y>right_defenders[1].y+paddle_radius.y) {
                    move_players(right_defenders, -ai_speed * elapsed);
                } else if (dfball_y<right_defenders[1].y-paddle_radius.y && dfball_y>right_defenders[2].y+paddle_radius.y) {
                    move_players(right_defenders, -ai_speed * elapsed);
                }
            } else {
                if (right_defenders[0].y < dfball_y) {
                    move_players(right_defenders, ai_speed * elapsed);
                } else if (right_defenders[2].y > dfball_y) {
                    move_players(right_defenders, -ai_speed * elapsed);
                } else if (dfball_y<right_defenders[0].y-paddle_radius.y && dfball_y>right_defenders[1].y+paddle_radius.y) {
                    move_players(right_defenders, ai_speed * elapsed);
                } else if (dfball_y<right_defenders[1].y-paddle_radius.y && dfball_y>right_defenders[2].y+paddle_radius.y) {
                    move_players(right_defenders, ai_speed * elapsed);
                }
            }
            if (stball_y>0) {
                if (right_strikers[0].y < stball_y) {
                    move_players(right_strikers, ai_speed * elapsed);
                } else if (right_strikers[1].y > stball_y) {
                    move_players(right_strikers, -ai_speed * elapsed);
                } else if (stball_y<right_strikers[0].y-paddle_radius.y && stball_y>right_strikers[1].y+paddle_radius.y) {
                    move_players(right_strikers, -ai_speed * elapsed);
                }
            } else {
                if (right_strikers[0].y < stball_y) {
                    move_players(right_strikers, ai_speed * elapsed);
                } else if (right_strikers[1].y > stball_y) {
                    move_players(right_strikers, -ai_speed * elapsed);
                } else if (stball_y<right_strikers[0].y-paddle_radius.y && stball_y>right_strikers[1].y+paddle_radius.y) {
                    move_players(right_strikers, ai_speed * elapsed);
                }
            }
            unblock_right_defenders = false;
            unblock_right_strikers = true;
		} else {
            if (dfball_y>0) {
                if (right_defenders[0].y < dfball_y) {
                    move_players(right_defenders, ai_speed * elapsed);
                } else if (right_defenders[2].y > dfball_y) {
                    move_players(right_defenders, -ai_speed * elapsed);
                } else if (ball.y<right_defenders[0].y-paddle_radius.y && ball.y>right_defenders[1].y+paddle_radius.y) {
                    move_players(right_defenders, -ai_speed * elapsed);
                } else if (ball.y<right_defenders[1].y-paddle_radius.y && ball.y>right_defenders[2].y+paddle_radius.y) {
                    move_players(right_defenders, -ai_speed * elapsed);
                }
            } else {
                if (right_defenders[0].y < dfball_y) {
                    move_players(right_defenders, ai_speed * elapsed);
                } else if (right_defenders[2].y > dfball_y) {
                    move_players(right_defenders, -ai_speed * elapsed);
                } else if (dfball_y<right_defenders[0].y-paddle_radius.y && dfball_y>right_defenders[1].y+paddle_radius.y) {
                    move_players(right_defenders, ai_speed * elapsed);
                } else if (dfball_y<right_defenders[1].y-paddle_radius.y && dfball_y>right_defenders[2].y+paddle_radius.y) {
                    move_players(right_defenders, ai_speed * elapsed);
                }
            }
            unblock_right_defenders = true;
            unblock_right_strikers = true;
		}

	}

    if (ball_velocity.x!=0){
        if (ball.x>left_defenders[0].x-paddle_radius.x && (ball_velocity.x < 0 || ball.x>left_defenders[0].x+paddle_radius.x)) {
            if (autod_pressed) q_pressed = false;
        } else {
            if (autod_pressed) q_pressed = true;
        }
    }

	if (w_pressed) {
	    if (shift_pressed) {
	        move_players(left_defenders, 2*court_radius.y * elapsed);
	    } else {
            move_players(left_defenders, court_radius.y * elapsed);
	    }
	}
	if (up_pressed) {
        if (shift_pressed) {
            move_players(left_strikers, 2*court_radius.y * elapsed);
        } else {
            move_players(left_strikers, court_radius.y * elapsed);
        }
	}
    if (s_pressed) {
        if (shift_pressed) {
            move_players(left_defenders, -2*court_radius.y * elapsed);
        } else {
            move_players(left_defenders, -court_radius.y * elapsed);
        }
    }
    if (down_pressed) {
        if (shift_pressed) {
            move_players(left_strikers, -2*court_radius.y * elapsed);
        } else {
            move_players(left_strikers, -court_radius.y * elapsed);
        }
    }

	//----- ball update -----

    speed_multiplier *= std::pow(0.95, elapsed);
    speed_multiplier = std::max(speed_multiplier, 2.5f);
	ball += elapsed * speed_multiplier * ball_velocity;


	//---- collision handling ----

	//paddles:
	auto paddle_vs_ball = [this](glm::vec2 const &paddle, std::string const kind, float elapsed) {
        auto calc_ball_vel = [this](glm::vec2 &speed, float x, float y) {
            float tot = std::sqrt(std::pow(x,2)+std::pow(y,2));
            speed.x = velocity * x/tot;
            speed.y = velocity * y/tot;
        };
		glm::vec2 min = glm::max(paddle - paddle_radius, ball - ball_radius);
		glm::vec2 max = glm::min(paddle + paddle_radius, ball + ball_radius);


		if (min.x > max.x || min.y > max.y) return;

		if (space_pressed == 1 || (space_pressed == 2 && speed_multiplier <= 3.2)) {
		    ball_velocity.x = 0;
            ball_velocity.y = 0;
            ball.x = paddle.x + paddle_radius.x + ball_radius.x;
		    return;
		}

        if (return_pressed) {
            calc_ball_vel(ball_velocity, paddle_radius.y + ball_radius.y, ball.y - paddle.y);
            speed_multiplier = 4.0f;
            if (kind == "ld") q_pressed = false;
            else e_pressed = false;
        }

        if (ball_velocity.x == 0 && ball_velocity.y == 0 && shift_pressed) {
            if (kind == "ld" && w_pressed) {
                ball.y = std::min(court_radius.y-ball_radius.y, ball.y+2*court_radius.y*elapsed);
            } else if (kind == "ld" && s_pressed) {
                ball.y = std::min(court_radius.y-ball_radius.y, ball.y-2*court_radius.y*elapsed);
            }
        }
        if (ball_velocity.x == 0 && ball_velocity.y == 0 && shift_pressed) {
            if (kind == "ls" && up_pressed) {
                ball.y = std::min(court_radius.y-ball_radius.y, ball.y+2*court_radius.y*elapsed);
            } else if (kind == "ls" && down_pressed) {
                ball.y = std::min(court_radius.y-ball_radius.y, ball.y-2*court_radius.y*elapsed);
            }
        }
        if (kind == "ld" && q_pressed) return;
        if (kind == "ls" && e_pressed) return;

		if (max.x - min.x > max.y - min.y) {
			if (ball.y > paddle.y) {
				ball.y = paddle.y + paddle_radius.y + ball_radius.y;
				ball_velocity.y = std::abs(ball_velocity.y);
			} else {
				ball.y = paddle.y - paddle_radius.y - ball_radius.y;
				ball_velocity.y = -std::abs(ball_velocity.y);
			}
		} else {
			if (ball.x > paddle.x) {
				ball.x = paddle.x + paddle_radius.x + ball_radius.x;
				ball_velocity.x = std::abs(ball_velocity.x);
			} else {
				ball.x = paddle.x - paddle_radius.x - ball_radius.x;
				ball_velocity.x = -std::abs(ball_velocity.x);
			}
			float vel = (ball.y - paddle.y) / (paddle_radius.y + ball_radius.y);
			ball_velocity.y = glm::mix(ball_velocity.y, vel, 0.75f);
		}
	};

	for (auto &def: left_defenders) {
        paddle_vs_ball(def, "ld", elapsed);
	}
    for (auto &str: left_strikers) {
        paddle_vs_ball(str, "ls", elapsed);
    }


    auto ai_vs_ball = [this](std::vector<glm::vec2> const &paddles, std::string kind, int i) {
        //compute area of overlap:
        auto calc_ball_vel = [this](glm::vec2 &speed, float x, float y) {
            float tot = std::sqrt(std::pow(x,2)+std::pow(y,2));
            speed.x = velocity * x/tot;
            speed.y = velocity * y/tot;
        };
        glm::vec2 min = glm::max(paddles[i] - paddle_radius, ball - ball_radius);
        glm::vec2 max = glm::min(paddles[i] + paddle_radius, ball + ball_radius);

        //if no overlap, no collision:

        if (min.x > max.x || min.y > max.y) return;

        float dist_to_lower = paddles[1].y - (-court_radius.y) - paddle_radius.y ;
        float dist_bet_paddles = paddles[0].y - 2*paddle_radius.y - paddles[1].y;
        float dist_to_upper = court_radius.y - paddles[0].y - paddle_radius.y;

        if (kind == "rd" && (unblock_right_defenders || ball_velocity.x == 0)) {
            if (i == 2) {
                if (dist_bet_paddles<=dist_to_lower) {
                    calc_ball_vel(ball_velocity, -(paddles[i].x-left_strikers[0].x), -court_radius.y+ball.y);
                } else {
                    calc_ball_vel(ball_velocity, -(paddles[i].x-left_strikers[0].x), (paddles[0].y+paddles[1].y)/2-ball.y);
                }
                speed_multiplier = 4.0f;
            } else if (i==0) {
                ball_velocity.x = -1.0f;
                if (dist_bet_paddles<=dist_to_upper) {
                    calc_ball_vel(ball_velocity, -(paddles[i].x-left_strikers[0].x), court_radius.y-ball.y);
                } else {
                    calc_ball_vel(ball_velocity, -(paddles[i].x-left_strikers[0].x), (paddles[0].y+paddles[1].y)/2-ball.y);
                }
                speed_multiplier = 4.0f;
            } else {
                calc_ball_vel(ball_velocity, -(paddles[i].x-left_strikers[0].x), (paddles[0].y+paddles[1].y)/2-ball.y);
            }
            return;
        }

        if (kind == "rs" && unblock_right_strikers) {
            calc_ball_vel(ball_velocity, -(ball.x+court_radius.x), -ball.y);
            speed_multiplier = 4.0f;
            return;
        }

        if (max.x - min.x > max.y - min.y) {
            if (ball.y > paddles[i].y) {
                ball.y = paddles[i].y + paddle_radius.y + ball_radius.y;
                ball_velocity.y = std::abs(ball_velocity.y);
            } else {
                ball.y = paddles[i].y - paddle_radius.y - ball_radius.y;
                ball_velocity.y = -std::abs(ball_velocity.y);
            }
        } else {
            if (ball.x > paddles[i].x) {
                ball.x = paddles[i].x + paddle_radius.x + ball_radius.x;
                ball_velocity.x = std::abs(ball_velocity.x);
            } else {
                ball.x = paddles[i].x - paddle_radius.x - ball_radius.x;
                ball_velocity.x = -std::abs(ball_velocity.x);
            }
            float vel = (ball.y - paddles[i].y) / (paddle_radius.y + ball_radius.y);
            ball_velocity.y = glm::mix(ball_velocity.y, vel, 0.75f);
        }
    };

    for (int i = 0; i<right_defenders.size(); i++) {
        ai_vs_ball(right_defenders, "rd", i);
    }
    for (int i = 0; i<right_strikers.size(); i++) {
        ai_vs_ball(right_strikers, "rs", i);
    }

    if (space_pressed == 1) {
        space_pressed = 2;
    }

	//court walls:
	if (ball.x < -court_radius.x + ball_radius.x && ball.y+ball_radius.y<net_radius && ball.y-ball_radius.y>-net_radius){
	    right_score+=1;
	    celebration = 2.0f;
	    ball_velocity.x = 0;
	    ball_velocity.y = 0;
	    scored = 1;
	    return;
	}
    if (ball.x > court_radius.x - ball_radius.x && ball.y+ball_radius.y<net_radius && ball.y-ball_radius.y>-net_radius){
        left_score+=1;
        celebration = 2.0f;
        ball_velocity.x = 0;
        ball_velocity.y = 0;
        scored = 0;
        return;
    }

    if (ball.y > court_radius.y - ball_radius.y) {
		ball.y = court_radius.y - ball_radius.y;
		if (ball_velocity.y > 0.0f) {
			ball_velocity.y = -ball_velocity.y;
		}
	}
	if (ball.y < -court_radius.y + ball_radius.y) {
		ball.y = -court_radius.y + ball_radius.y;
		if (ball_velocity.y < 0.0f) {
			ball_velocity.y = -ball_velocity.y;
		}
	}

	if (ball.x > court_radius.x - ball_radius.x) {
		ball.x = court_radius.x - ball_radius.x;
		if (ball_velocity.x > 0.0f) {
			ball_velocity.x = -ball_velocity.x;
		}
	}
	if (ball.x < -court_radius.x + ball_radius.x) {
		ball.x = -court_radius.x + ball_radius.x;
		if (ball_velocity.x < 0.0f) {
			ball_velocity.x = -ball_velocity.x;
		}
	}

}

void FoosballMode::draw(glm::uvec2 const &drawable_size) {
	//some nice colors from the course web page:
	#define HEX_TO_U8VEC4( HX ) (glm::u8vec4( (HX >> 24) & 0xff, (HX >> 16) & 0xff, (HX >> 8) & 0xff, (HX) & 0xff ))
	const glm::u8vec4 bg_color = HEX_TO_U8VEC4(0x175514ff);
	const glm::u8vec4 fg_color = HEX_TO_U8VEC4(0xff0000ff);
    const glm::u8vec4 unblock_color = HEX_TO_U8VEC4(0xff00004c);
    const glm::u8vec4 opposing_color = HEX_TO_U8VEC4(0x0000ffff);
    const glm::u8vec4 unpposing_color = HEX_TO_U8VEC4(0x0000ff4c);
    const glm::u8vec4 white_color = HEX_TO_U8VEC4(0xffffffff);
	#undef HEX_TO_U8VEC4

	//other useful drawing constants:
	const float wall_radius = 0.05f;
//	const float shadow_offset = 0.07f;
	const float padding = 0.14f; //padding between outside of walls and edge of window

	//---- compute vertices to draw ----

	//vertices will be accumulated into this list and then uploaded+drawn at the end of this function:
	std::vector< Vertex > vertices;


	//inline helper function for rectangle drawing:
	auto draw_rectangle = [&vertices](glm::vec2 const &center, glm::vec2 const &radius, glm::u8vec4 const &color) {
		//draw rectangle as two CCW-oriented triangles:
		vertices.emplace_back(glm::vec3(center.x-radius.x, center.y-radius.y, 0.0f), color, glm::vec2(0.5f, 0.5f));
		vertices.emplace_back(glm::vec3(center.x+radius.x, center.y-radius.y, 0.0f), color, glm::vec2(0.5f, 0.5f));
		vertices.emplace_back(glm::vec3(center.x+radius.x, center.y+radius.y, 0.0f), color, glm::vec2(0.5f, 0.5f));
		vertices.emplace_back(glm::vec3(center.x-radius.x, center.y-radius.y, 0.0f), color, glm::vec2(0.5f, 0.5f));
		vertices.emplace_back(glm::vec3(center.x+radius.x, center.y+radius.y, 0.0f), color, glm::vec2(0.5f, 0.5f));
		vertices.emplace_back(glm::vec3(center.x-radius.x, center.y+radius.y, 0.0f), color, glm::vec2(0.5f, 0.5f));
	};

	//walls:
    draw_rectangle(glm::vec2(0.0f, 0.0f), glm::vec2(wall_radius, court_radius.y + 2.0f * wall_radius), white_color);
	draw_rectangle(glm::vec2(-court_radius.x-wall_radius, 0.0f), glm::vec2(wall_radius, court_radius.y + 2.0f * wall_radius), white_color);
	draw_rectangle(glm::vec2( court_radius.x+wall_radius, 0.0f), glm::vec2(wall_radius, court_radius.y + 2.0f * wall_radius), white_color);
	draw_rectangle(glm::vec2( 0.0f,-court_radius.y-wall_radius), glm::vec2(court_radius.x, wall_radius), white_color);
	draw_rectangle(glm::vec2( 0.0f, court_radius.y+wall_radius), glm::vec2(court_radius.x, wall_radius), white_color);

    draw_rectangle(glm::vec2(-court_radius.x-wall_radius-2.0f, 0.0f), glm::vec2(2.0f+wall_radius, net_radius), white_color);
    draw_rectangle(glm::vec2(court_radius.x+wall_radius+2.0f, 0.0f), glm::vec2(2.0f+wall_radius, net_radius), white_color);

	//paddles:
	if (q_pressed) {
        for (auto def: left_defenders) {
            draw_rectangle(def, paddle_radius, unblock_color);
        }
    }else{
        for (auto def: left_defenders) {
            draw_rectangle(def, paddle_radius, fg_color);
        }

    }
    if (e_pressed) {
        for (auto str: left_strikers) {
            draw_rectangle(str, paddle_radius, unblock_color);
        }
	} else {

        for (auto str: left_strikers) {
            draw_rectangle(str, paddle_radius, fg_color);
        }
	}

    if (unblock_right_strikers) {
        for (auto str: right_strikers) {
            draw_rectangle(str, paddle_radius, unpposing_color);
        }
    } else {
        for (auto str: right_strikers) {
            draw_rectangle(str, paddle_radius, opposing_color);
        }
    }

    if (unblock_right_defenders) {
        for (auto def: right_defenders) {
            draw_rectangle(def, paddle_radius, unpposing_color);
        }
    } else {
        for (auto def: right_defenders) {
            draw_rectangle(def, paddle_radius, opposing_color);
        }
    }
	//ball:
	draw_rectangle(ball, ball_radius, white_color);

	//scores:
	glm::vec2 score_radius = glm::vec2(0.1f, 0.1f);
	for (uint32_t i = 0; i < left_score; ++i) {
		draw_rectangle(glm::vec2( -court_radius.x + (2.0f + 3.0f * i) * score_radius.x, court_radius.y + 2.0f * wall_radius + 2.0f * score_radius.y), score_radius, fg_color);
	}
	for (uint32_t i = 0; i < right_score; ++i) {
		draw_rectangle(glm::vec2( court_radius.x - (2.0f + 3.0f * i) * score_radius.x, court_radius.y + 2.0f * wall_radius + 2.0f * score_radius.y), score_radius, opposing_color);
	}

	//------ compute court-to-window transform ------

	//compute area that should be visible:
	glm::vec2 scene_min = glm::vec2(
		-court_radius.x - 2.0f * wall_radius - padding,
		-court_radius.y - 2.0f * wall_radius - padding
	);
	glm::vec2 scene_max = glm::vec2(
		court_radius.x + 2.0f * wall_radius + padding,
		court_radius.y + 2.0f * wall_radius + 3.0f * score_radius.y + padding
	);

	//compute window aspect ratio:
	float aspect = drawable_size.x / float(drawable_size.y);
	//we'll scale the x coordinate by 1.0 / aspect to make sure things stay square.

	//compute scale factor for court given that...
	float scale = std::min(
		(2.0f * aspect) / (scene_max.x - scene_min.x), //... x must fit in [-aspect,aspect] ...
		(2.0f) / (scene_max.y - scene_min.y) //... y must fit in [-1,1].
	);

	glm::vec2 center = 0.5f * (scene_max + scene_min);

	//build matrix that scales and translates appropriately:
	glm::mat4 court_to_clip = glm::mat4(
		glm::vec4(scale / aspect, 0.0f, 0.0f, 0.0f),
		glm::vec4(0.0f, scale, 0.0f, 0.0f),
		glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
		glm::vec4(-center.x * (scale / aspect), -center.y * scale, 0.0f, 1.0f)
	);
	//NOTE: glm matrices are specified in *Column-Major* order,
	// so each line above is specifying a *column* of the matrix(!)

	//also build the matrix that takes clip coordinates to court coordinates (used for mouse handling):
	clip_to_court = glm::mat3x2(
		glm::vec2(aspect / scale, 0.0f),
		glm::vec2(0.0f, 1.0f / scale),
		glm::vec2(center.x, center.y)
	);

	//---- actual drawing ----

	//clear the color buffer:
	glClearColor(bg_color.r / 255.0f, bg_color.g / 255.0f, bg_color.b / 255.0f, bg_color.a / 255.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//use alpha blending:
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//don't use the depth test:
	glDisable(GL_DEPTH_TEST);

	//upload vertices to vertex_buffer:
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer); //set vertex_buffer as current
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), vertices.data(), GL_STREAM_DRAW); //upload vertices array
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//set color_texture_program as current program:
	glUseProgram(color_texture_program.program);

	//upload OBJECT_TO_CLIP to the proper uniform location:
	glUniformMatrix4fv(color_texture_program.OBJECT_TO_CLIP_mat4, 1, GL_FALSE, glm::value_ptr(court_to_clip));

	//use the mapping vertex_buffer_for_color_texture_program to fetch vertex data:
	glBindVertexArray(vertex_buffer_for_color_texture_program);

	//bind the solid white texture to location zero so things will be drawn just with their colors:
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, white_tex);

	//run the OpenGL pipeline:
	glDrawArrays(GL_TRIANGLES, 0, GLsizei(vertices.size()));

	//unbind the solid white texture:
	glBindTexture(GL_TEXTURE_2D, 0);

	//reset vertex array to none:
	glBindVertexArray(0);

	//reset current program to none:
	glUseProgram(0);
	

	GL_ERRORS(); //PARANOIA: print errors just in case we did something wrong.

}
