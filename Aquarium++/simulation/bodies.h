//
// Created by paulo on 25/06/2023.
//

#ifndef SDL_PLAYGROUND_BODIES_H
#define SDL_PLAYGROUND_BODIES_H

#include <SDL.h>
#include "vectors.h"

#define ROTATE_TEXTURE 1
#define DEBUG_COLLISIONS 0

#define WANDER_RATE 10

#define WALL_CLOSENESS_FEAR 75

class Body {
public:
	int32_t health;
	
	double mass;
	double max_force;
	
	double max_speed;
	double wander_speed;
	double current_speed;
	
	SDL_Rect collision_rect;
	
	SDL_Rect rect;
	SDL_Texture* texture;
	
	Vector2 position = {0, 0};
	Vector2 velocity = {0, 0};
	
	Vector2 obstacle_check_lines[3] = {{0, 0}, {0, 0}, {0, 0}};
	
	Body(Vector2 position, double mass, SDL_Texture* texture, double max_speed) {
		this->health = 100;
		this->mass = mass;
		this->max_force = 100 / mass;
		
		
		this->max_speed = max_speed;
		this->wander_speed = max_speed / 5;
		this->current_speed = 0;
		
		this->collision_rect.x = (int) (position.x + ((rect.w / 4.0) * (velocity.x > 0 ? 1 : -1)));
		this->collision_rect.y = (int) (position.y + ((rect.h / 4.0) * (velocity.y > 0 ? 1 : -1)));
		this->collision_rect.w = rect.w / 2;
		this->collision_rect.h = rect.h / 2;
		this->rect.x = (int) (position.x - (mass / 2));
		this->rect.y = (int) (position.y - (mass / 2));
		this->rect.w = (int) mass;
		this->rect.h = (int) mass;
		
		this->texture = texture;
		
		this->position = position;
		
		for (auto &obstacle_check_line: this->obstacle_check_lines) {
			obstacle_check_line = Vector2(0, 0);
		}
	}
	
	void steer_to(Vector2 steer_direction, double clamp_speed) {
		if ((steer_direction - this->position).length_sqr() == 0) {
			return;
		}
		
		Vector2 accel = steer_direction.clamp(this->max_force).scale(1 / this->mass);
		Vector2 result_vec = this->velocity + accel;
		
		if (this->current_speed < clamp_speed) {
			this->current_speed += 0.06;
			clamp_speed = this->current_speed;
		} else if (this->current_speed > clamp_speed) {
			this->current_speed -= 0.06;
			clamp_speed = this->current_speed;
		}
		
		this->velocity = result_vec.clamp(clamp_speed);
		this->position = this->position + this->velocity;
		
		this->rect.x = (int) this->position.x - (this->rect.w / 2);
		this->rect.y = (int) this->position.y - (this->rect.h / 2);
		
		Vector2 norm_vel = this->velocity.normalize();
		this->collision_rect.x = (int) (this->position.x + ((this->rect.w / 4.0) * norm_vel.x) -
										(this->collision_rect.w / 2.0));
		this->collision_rect.y = (int) (this->position.y + ((this->rect.h / 4.0) * norm_vel.y) -
										(this->collision_rect.w / 2.0));
		
	}
	
	void seek(Vector2 target) {
		Vector2 to_tgt_vec = target - this->position;
		Vector2 desired_vel = to_tgt_vec.clamp(this->max_force);
		this->steer_to(desired_vel - this->velocity, this->max_speed);
	}
	
	void arrive(Vector2 target) {
		Vector2 to_tgt_vec = target - this->position;
		double dist = to_tgt_vec.length();
		double ramped_speed = this->max_speed * (dist / 100);
		double clipped_speed = fmin(ramped_speed, this->max_speed);
		Vector2 desired_vel = to_tgt_vec.scale(clipped_speed / dist);
		this->steer_to(desired_vel - this->velocity, this->max_speed);
	}
	
	void wander(void) {
		Vector2 wander_change = {this->velocity.x + ((((rand() % (WANDER_RATE + 1)) - (WANDER_RATE / 2.0)) / 10)),
								 this->velocity.y + (((rand() % (WANDER_RATE + 1)) - (WANDER_RATE / 2.0)) / 10)};
		this->steer_to(wander_change - this->velocity, this->wander_speed);
	}
	
	void flee(Vector2 target) {
		Vector2 desired_vel = (this->position - target).normalize().scale(this->max_speed);
		this->steer_to(desired_vel - this->velocity, this->max_speed);
	}
	
	void grow(double mass_growth) {
		this->mass += mass_growth;
		this->rect.h = (int) (5 * sqrt(this->mass));
		this->rect.w = (int) (5 * sqrt(this->mass));
		
		this->collision_rect.w = this->rect.w / 2;
		this->collision_rect.h = this->rect.h / 2;
	}
	
	bool keep_inside(SDL_Rect boundary) {
		Vector2 future_pos = this->position + this->velocity.scale(WALL_CLOSENESS_FEAR);
		
		if ((future_pos.x > boundary.x && future_pos.x < boundary.w) &&
			(future_pos.y > boundary.y && future_pos.y < boundary.h)) {
			return false;
		}
		
		this->seek(Vector2(boundary.w / 2.0, boundary.h / 2.0));
		
		return true;
	}
	
	void draw(SDL_Renderer* rend) {
#if ROTATE_TEXTURE == 1
		double angle = rag_to_deg(atan2(this->velocity.y, this->velocity.x));
		SDL_RenderCopyEx(rend, this->texture, nullptr, &this->rect, angle, nullptr,
						 ((angle > 90) && (angle < 270)) ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE);
#else
		SDL_RenderCopy(rend, this->texture, NULL, &this->rect);
#endif

#if DEBUG_COLLISIONS == 1
		uint8_t r;
	uint8_t g;
	uint8_t b;
	SDL_GetRenderDrawColor(rend, &r, &g, &b, NULL);
	SDL_SetRenderDrawColor(rend, 255, 0, 0, 0);
	SDL_RenderDrawRect(rend, &this->rect);
	SDL_SetRenderDrawColor(rend, 0, 255, 255, 0);
	SDL_RenderDrawRect(rend, &this->collision_rect);
	SDL_SetRenderDrawColor(rend, r, g, b, 0);
#endif
	}
	
};

class BodyNode {
public:
	Body* body;
	BodyNode* next;
	BodyNode* prev;
	
	explicit BodyNode(Body* body, BodyNode* previous_node = nullptr, BodyNode* next_node = nullptr) {
		this->body = body;
		this->prev = previous_node;
		this->next = next_node;
	}
};

class BodyList {
private:
	BodyNode* list_head;

public:
	
	explicit BodyList(BodyNode* head) {
		this->list_head = head;
	}
	
	BodyNode* get_head() {
		return list_head;
	}
	
	Body* get_closest_body_from(const Body* origin, double detection_radius) {
		uint32_t dist;
		uint32_t min_dist = SDL_MAX_SINT32;
		BodyNode* current_node = this->list_head;
		Body* closest_body = nullptr;
		
		while (current_node != nullptr) {
			dist = (uint32_t) (current_node->body->position - origin->position).length();
			if ((dist < detection_radius) && dist < min_dist) {
				min_dist = dist;
				closest_body = current_node->body;
			}
			
			current_node = current_node->next;
		}
		
		return closest_body;
	}
	
	void insert_node(BodyNode* node_to_insert) {
		node_to_insert->next = this->list_head;
		if (this->list_head != nullptr) {
			this->list_head->prev = node_to_insert;
		}
		this->list_head = node_to_insert;
	}
	
	void delete_node(BodyNode* node_to_delete) {
		if (node_to_delete->next) {
			node_to_delete->next->prev = node_to_delete->prev;
		}
		
		if (node_to_delete->prev) {
			node_to_delete->prev->next = node_to_delete->next;
		} else {
			this->list_head = node_to_delete->next;
		}
		
		free(node_to_delete);
	}
};

#endif //SDL_PLAYGROUND_BODIES_H