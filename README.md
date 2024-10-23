# WebSocketActor

## Description

**WebSocketActor** is a C++ Unreal Engine Actor class that facilitates real-time communication between your Unreal Engine application and a WebSocket server. It enables your game or application to send and receive messages over WebSockets, handling both text and binary data efficiently. This actor is particularly useful for applications that require real-time data streaming, such as multiplayer games, live updates, or dynamic content loading.

## Features

- **WebSocket Connectivity**: Establishes a WebSocket connection to a specified server and group ID.
- **Text Message Handling**: Receives text messages from the server and displays information on-screen.
- **Binary Data Processing**: Accumulates binary data fragments, converts them into textures, and applies them to materials dynamically.
- **Dynamic Texture Application**: Updates materials in the game world with textures received over WebSockets.
- **Debugging and Logging**: Provides on-screen debug messages and extensive logging for easier development and troubleshooting.

## Installation

To integrate **WebSocketActor** into your Unreal Engine project, follow these steps:

1. **Clone or Download**: Clone this repository or download the `WebSocketActor.h` and `WebSocketActor.cpp` files.

2. **Add to Project**: Place the `WebSocketActor.h` and `WebSocketActor.cpp` files into your project's `Source/[YourProjectName]/` directory.

3. **Enable WebSockets Module**:

   - Open your project's `.Build.cs` file (e.g., `[YourProjectName].Build.cs`).
   - Add `"WebSockets"`, `"Sockets"`, `"Networking"`, `"OpenSSL"`, `"ImageWrapper"`, `"Json"` to the list of `PublicDependencyModuleNames`. It should look like this:

     ```csharp
     PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "WebSockets", "Sockets", "Networking", "OpenSSL", "ImageWrapper", "Json" });
     ```

4. **Rebuild Project**: Rebuild your project to include the new actor and dependencies.

## Usage

To use the **WebSocketActor** in your project:

1. **Place Actor in Level**:

   - In the Unreal Editor, open your level.
   - Drag and drop an instance of **WebSocketActor** into the level.

2. **Configure Properties**:

   - Select the **WebSocketActor** instance in the level.
   - In the **Details** panel, configure the following properties:

     - **WebSocketURL**: The base URL of your WebSocket server (e.g., `ws://127.0.0.1/ws/signaling/game_instance/`).
     - **WebSocketGroupID**: The group ID or endpoint to connect to (e.g., `ASDG`).
     - **DynamicMaterialInstance**: Assign a material instance that will receive the dynamic textures.

3. **Assign Material to Mesh**:

   - Ensure that you have a mesh in your scene with a material that uses a texture parameter (e.g., `DynamicTextureParam`).
   - Create a dynamic material instance of that material.
   - Assign the dynamic material instance to the **DynamicMaterialInstance** property of the **WebSocketActor**.

4. **Run the Project**:

   - Start your project.
   - The **WebSocketActor** will attempt to connect to the specified WebSocket server.
   - Upon receiving messages, it will handle them according to the implemented logic.

### Example

Below is an example of how you might set up the **WebSocketActor** in code.

```cpp
// In your level's actor class or game mode

void AMyLevelActor::BeginPlay()
{
    Super::BeginPlay();

    // Spawn the WebSocketActor
    FActorSpawnParameters SpawnParams;
    AWebSocketActor* WebSocketActorInstance = GetWorld()->SpawnActor<AWebSocketActor>(AWebSocketActor::StaticClass(), SpawnParams);

    // Configure the WebSocketActor
    WebSocketActorInstance->WebSocketURL = TEXT("ws://127.0.0.1/ws/signaling/game_instance/");
    WebSocketActorInstance->WebSocketGroupID = TEXT("ASDG");

    // Create a dynamic material instance
    UMaterialInstanceDynamic* DynMaterialInstance = UMaterialInstanceDynamic::Create(BaseMaterial, this);
    WebSocketActorInstance->DynamicMaterialInstance = DynMaterialInstance;

    // Apply the dynamic material to a mesh
    MyMeshComponent->SetMaterial(0, DynMaterialInstance);
}
```

### Handling Messages

The **WebSocketActor** comes with predefined methods for handling different types of messages:

- **OnConnected**: Called when the WebSocket connection is successfully established.
- **OnConnectionError**: Called when there is an error connecting to the WebSocket server.
- **OnMessage**: Handles incoming text messages.
- **OnBinaryMessage**: Handles incoming binary messages, accumulates data fragments, and converts them to textures.
- **OnClosed**: Called when the WebSocket connection is closed.

You can customize these methods to handle messages according to your application's needs.

## License

This project is licensed as **Free**. You are free to use, modify, and distribute this code.

## Contributing

Contributions are welcome! If you have suggestions for improvements or have found bugs, please open an issue to discuss them. Pull requests are also welcome.

To contribute:

1. **Fork the Repository**: Click on the 'Fork' button at the top right of this page to create a copy of this repository on your GitHub account.

2. **Create a Branch**: Create a new branch in your forked repository for your feature or bug fix.

   ```bash
   git checkout -b feature/your-feature-name
   ```

3. **Commit Your Changes**: Make your changes and commit them with clear and descriptive commit messages.

   ```bash
   git commit -m "Add feature XYZ"
   ```

4. **Push to GitHub**: Push your changes to your forked repository.

   ```bash
   git push origin feature/your-feature-name
   ```

5. **Open a Pull Request**: Navigate to the original repository and open a pull request from your branch.

## Contact

If you have any questions or need assistance, feel free to open an issue or reach out directly.

---
