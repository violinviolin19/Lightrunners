#include "AIController.h"

#define MIN_DISTANCE 300
#define HEALTH_LIM 25
#define ATTACK_RANGE 100

#pragma mark EnemyController

EnemyController::EnemyController(){};

void EnemyController::idling() { _enemy->move(0, 0); }

void EnemyController::chasePlayer(const cugl::Vec2 p) {
  cugl::Vec2 diff = p - _enemy->getPosition();
  diff.subtract(_enemy->getVX(), _enemy->getVY());
  diff.add(_enemy->getVX(), _enemy->getVY());
  diff.scale(_enemy->getSpeed());
  _enemy->move(diff.x, diff.y);
}

void EnemyController::attackPlayer(const cugl::Vec2 p) {
  if (_enemy->getAttackCooldown() <= 0) {
    _enemy->addBullet(p);
    _enemy->setAttackCooldown(120);
  }
  _enemy->move(0, 0);
}

void EnemyController::avoidPlayer(const cugl::Vec2 p) {
  cugl::Vec2 diff = p - _enemy->getPosition();
  diff.subtract(_enemy->getVX(), _enemy->getVY());
  diff.add(_enemy->getVX(), _enemy->getVY());
  diff.scale(_enemy->getSpeed() / 2);
  _enemy->move(-diff.x, -diff.y);
}

bool EnemyController::init(const cugl::Vec2 pos, string name,
                           std::shared_ptr<cugl::AssetManager> assets,
                           float tile_height, float row_count) {
  std::shared_ptr<cugl::Texture> grunt_texture =
      assets->get<cugl::Texture>("grunt");
  _enemy = Grunt::alloc(pos, name);

  auto grunt_node = cugl::scene2::SpriteNode::alloc(grunt_texture, 1, 1);
  _enemy->setGruntNode(grunt_node);
  grunt_node->setPriority(
      100);  // TODO: Update priority according to position on screen

  _projectile_texture = assets->get<cugl::Texture>("projectile");

  _current_state = IDLE;

  _tile_height = tile_height;
  _row_count = row_count;

  return true;
}

void EnemyController::update(
    float timestep, std::shared_ptr<Player> player,
    std::shared_ptr<cugl::physics2::ObstacleWorld> _world,
    std::shared_ptr<cugl::scene2::SceneNode> _world_node,
    std::shared_ptr<cugl::scene2::SceneNode> _debug_node) {
  
  // Change state if applicable
  float distance =
      (_enemy->getPosition()).subtract(player->getPosition()).length();
  changeStateIfApplicable(distance);

  // Perform player action
  cugl::Vec2 p = player->getPosition();
  performAction(p);

  // Reduce attack cooldown if enemy has attacked
  if (_enemy->getAttackCooldown() > 0) {
    _enemy->reduceAttackCooldown(1);
  }

  // Update enemy & projectiles
  updateProjectiles(timestep, _world, _world_node, _debug_node);
  _enemy->update(timestep);
  
  // Set the priority for drawing
  int row = (int)floor(_enemy->getBody()->GetPosition().y / _tile_height);
  _enemy->getGruntNode()->setPriority(_row_count - row);
}

void EnemyController::changeStateIfApplicable(float distance) {
  // Change state if applicable
  int health = _enemy->getHealth();
  if (health <= HEALTH_LIM) {
    _current_state = AVOIDING;
    if (distance > MIN_DISTANCE) {
      _current_state = IDLE;
    }
  } else {
    if (distance <= ATTACK_RANGE) {
      _current_state = ATTACKING;
    } else if (distance <= MIN_DISTANCE) {
      _current_state = CHASING;
    } else {
      _current_state = IDLE;
    }
  }
}

void EnemyController::performAction(cugl::Vec2 p) {
  switch (_current_state) {
    case IDLE: {
      idling();
      break;
    }
    case CHASING: {
      chasePlayer(p);
      break;
    }
    case ATTACKING: {
      attackPlayer(p);
      break;
    }
    case AVOIDING: {
      avoidPlayer(p);
      break;
    }
  }
}

void EnemyController::updateProjectiles(
    float timestep, std::shared_ptr<cugl::physics2::ObstacleWorld> _world,
    std::shared_ptr<cugl::scene2::SceneNode> _world_node,
    std::shared_ptr<cugl::scene2::SceneNode> _debug_node) {
  auto proj = _enemy->getProjectiles();
  auto itt = proj.begin();
  while (itt != proj.end()) {
    // Add to world if needed
    if ((*itt)->getNode() == nullptr) {
      _world->addObstacle((*itt));
      auto proj_node =
          cugl::scene2::SpriteNode::alloc(_projectile_texture, 1, 1);
      int row = (int)floor((*itt)->getBody()->GetPosition().y / _tile_height);
      proj_node->setPriority(_row_count - row);
      proj_node->setPosition((*itt)->getPosition());
      (*itt)->setNode(proj_node);
      _world_node->addChild(proj_node);
      (*itt)->setDebugScene(_debug_node);
      (*itt)->setDebugColor(cugl::Color4f::BLACK);
      (*itt)->setInWorld(true);
    }

    (*itt)->decrementFrame(1);
    (*itt)->getNode()->setPosition((*itt)->getPosition());
    ++itt;
  }

  _enemy->deleteProjectile(_world, _world_node);
}
