This is the project for the OS Course
Run the make command to compile.
Commands for the treasure_hub shell:
Available commands:
  start_monitor            - Start the monitor process
  stop_monitor             - Send stop signal to monitor
  list_hunts               - List all available hunts
  list_treasures <hunt_id> - List treasures in a hunt
  view_treasure <hunt_id> <treasure_id> - View specific treasure
  calculate_score          - Print scores for all hunts
  help                     - Show this help message
  exit                     - Exit the hub (only if monitor is stopped)
For the treasure_hunt manager:
Usage:
  ./treasure_hunt add <hunt_id>
  ./treasure_hunt list <hunt_id>
  ./treasure_hunt view <hunt_id> <treasure_id>
  ./treasure_hunt remove_treasure <hunt_id> <id>
  ./treasure_hunt remove_hunt <hunt_id>
