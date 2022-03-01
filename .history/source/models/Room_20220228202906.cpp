Room::Room(cugl::Size size)
    : type(Room::RoomType::DEFAULT), visited(false), fixed(false) {
  node = cugl::scene2::PolygonNode::alloc();
  node->setContentSize(size);
}

void Room::move(cugl::Vec2 dir) {
  if (!fixed) {
    dir.set(roundf(dir.x), roundf(dir.y));
    node->setPosition(node->getPosition() + dir);
  }
}

bool Room::hasNeighbor(const std::shared_ptr<Room> room) const {
  auto result = std::find_if(
      edges.begin(), edges.end(), [room](const std::shared_ptr<Edge> &edge) {
        return edge->source == room || edge->neighbor == room;
      });

  return result != edges.end();
}

std::shared_ptr<Edge> Room::findEdge(const std::shared_ptr<Room> room) const {
  auto result = std::find_if(
      edges.begin(), edges.end(), [room](const std::shared_ptr<Edge> &edge) {
        return edge->source == room || edge->neighbor == room;
      });

  return (result != edges.end()) ? *result : nullptr;
}