// Fill out your copyright notice in the Description page of Project Settings.


#include "TestNewWebSocketActor.h"
#include "WebSocketsModule.h"
#include "IWebSocket.h"

#include "Engine/Texture2D.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "IImageWrapperModule.h"
#include "IImageWrapper.h"
#include "ImageWrapperHelper.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/OutputDeviceDebug.h"
#include "Misc/ScopeLock.h"

#include "Json.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"


// Initialize static members
TArray<uint8> WebSocketActor::AccumulatedData;
FString PrevValue = "0";
//FCriticalSection WebSocketActor::Mutex;


void WebSocketActor::ResetAndLogMessageCount()
{
    // Log the number of messages received in the last second
    UE_LOG(LogTemp, Log, TEXT("Messages received in the last second: %d"), MessageCount);

    // Reset the message counter
    MessageCount = 0;
}

// Sets default values
WebSocketActor::WebSocketActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Default values
	WebSocketURL = TEXT("ws://127.0.0.1/ws/signaling/game_instance/");
    WebSocketGroupID = TEXT("ASDG");
    MessageCount = 0;
}

// Called when the game starts or when spawned
void WebSocketActor::BeginPlay()
{
	Super::BeginPlay();

    // Set a timer to call ResetAndLogMessageCount every 1 second
    // GetWorldTimerManager().SetTimer(MessageCountTimerHandle, this, &WebSocketActor::ResetAndLogMessageCount, 1.0f, true);

    if (!FModuleManager::Get().IsModuleLoaded("WebSockets"))
    {
        FModuleManager::Get().LoadModule("WebSockets");
    }

    FString WebSocketCompleteURL = FString::Format(*FString(TEXT("{0}{1}/")), { WebSocketURL, WebSocketGroupID });

    WebSocket = FWebSocketsModule::Get().CreateWebSocket(WebSocketCompleteURL);
    WebSocket->OnConnected().AddUObject(this, &WebSocketActor::OnConnected);
    WebSocket->OnConnectionError().AddUObject(this, &WebSocketActor::OnConnectionError);
    // Bind to the OnMessage event for text messages
    WebSocket->OnMessage().AddLambda([this](const FString& Message)
        {
            if (PrevValue != Message) {
                FString FormattedMessage = FString::Format(*FString(TEXT("Number of fingers raised is: {0}")), { *Message });
                GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FormattedMessage);
                PrevValue = Message;
            }
        });
    //WebSocket->OnMessage().AddUObject(this, &WebSocketActor::OnMessage);
    // Bind to the OnRawMessage event for binary or raw messages
    WebSocket->OnBinaryMessage().AddLambda([this](const void* Data, SIZE_T Size, bool bIsLastFragment)
        {
            //GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Binary message recived."));

            // Append the incoming data to the accumulated data array
            AccumulatedData.Append(static_cast<const uint8*>(Data), Size);

            // If this is the last fragment, log the complete accumulated data
            if (bIsLastFragment)
            {
                //GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Starting to convert in Binary."));
                ConvertBinaryToTexture(AccumulatedData);
                AccumulatedData.Empty();
            }
        });
    //WebSocket->OnBinaryMessage().AddUObject(this, &WebSocketActor::OnBinaryMessage);
    //WebSocket->OnRawMessage().AddUObject(this, &WebSocketActor::OnRawMessage);
    WebSocket->OnClosed().AddUObject(this, &WebSocketActor::OnClosed);

    WebSocket->Connect();
}

// Called every frame
void WebSocketActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void WebSocketActor::OnConnected()
{
    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "Successfully connected");
}

void WebSocketActor::OnConnectionError(const FString& Error)
{
    FString FormattedMessage = FString::Format(*FString(TEXT("WebSocket connection error: {0}")), { *Error });

    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FormattedMessage);
}

void WebSocketActor::OnMessage(const FString& Message)
{
    UE_LOG(LogTemp, Warning, TEXT("Received WebSocket Message: %s"), *Message);
    //FString FormattedMessage = FString::Format(*FString(TEXT("Received message: {0}")), { *Message });
    //GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FormattedMessage);
}

void WebSocketActor::OnBinaryMessage(const void* Data, SIZE_T Size, bool bIsLastFragment)
{

    UE_LOG(LogTemp, Warning, TEXT("Received WebSocket Binary Message."));
    //FString FormattedMessage = FString::Format(*FString(TEXT("Received binary message and remaining: {0}")), {bIsLastFragment});
    //GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, FormattedMessage);
    
    
    // Lock the critical section to ensure thread safety
    //FScopeLock Lock(&Mutex);

    // Append the incoming data to the accumulated data array
    AccumulatedData.Append(static_cast<const uint8*>(Data), Size);

    // If this is the last fragment, log the complete accumulated data
    if (bIsLastFragment)
    {
        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Starting to convert in Binary."));
        ConvertBinaryToTexture(AccumulatedData);
        AccumulatedData.Empty();
    }
    else {
        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Won't start to convert in Binary."));
    }
}

void WebSocketActor::OnRawMessage(const void* Data, SIZE_T Size, SIZE_T BytesRemaining)
{
    /*FString FormattedMessage = FString::Format(*FString(TEXT("Received Raw message and remaining: {0}")), {BytesRemaining});
    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, FormattedMessage);*/


    // Lock the critical section to ensure thread safety
    //FScopeLock Lock(&Mutex);

    // Append the incoming data to the accumulated data array
    AccumulatedData.Append(static_cast<const uint8*>(Data), Size);

    // If this is the last fragment, log the complete accumulated data
    if (BytesRemaining == 0)
    {
        // Handle the case where no bytes are remaining
        UE_LOG(LogTemp, Warning, TEXT("No bytes remaining."));
        FString AccumulatedDataHex;
        for (uint8 Byte : AccumulatedData)
        {
            AccumulatedDataHex += FString::Printf(TEXT("%02X "), Byte);
        }

        UE_LOG(LogTemp, Warning, TEXT("AccumulatedData: %s"), *AccumulatedDataHex);

        ConvertBinaryToTexture(AccumulatedData);
        AccumulatedData.Empty();
    }
    else
    {
        // Handle the case where there are still bytes remaining
        UE_LOG(LogTemp, Warning, TEXT("%llu bytes remaining."), BytesRemaining);
    }
}

void WebSocketActor::OnClosed(int32 StatusCode, const FString& Reason, bool bWasClean)
{
    FString FormattedMessage = FString::Format(*FString(TEXT("Connection closed: {0}")), { *Reason });
    GEngine->AddOnScreenDebugMessage(-1, 15.0f, bWasClean ? FColor::Green : FColor::Red, FormattedMessage);
}

void WebSocketActor::ConvertBinaryToTexture(const TArray<uint8>& BinaryData)
{
    UE_LOG(LogTemp, Warning, TEXT("Coverting binary to texture started."));
    // Load image from binary data
    IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
    TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

    if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(BinaryData.GetData(), BinaryData.Num()))
    {
        //UE_LOG(LogTemp, Warning, TEXT("Reading the image."));
        TArray<uint8> UncompressedBGRA;
        if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA))
        {
            //UE_LOG(LogTemp, Warning, TEXT("Uncompressing the image."));
            int a = 3;
            UTexture2D* Texture = UTexture2D::CreateTransient(
                ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);

            void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
            FMemory::Memcpy(TextureData, UncompressedBGRA.GetData(), UncompressedBGRA.Num());
            Texture->GetPlatformData()->Mips[0].BulkData.Unlock();

            Texture->UpdateResource();

            ApplyTextureToMaterial(Texture);
        }
        else {
            //UE_LOG(LogTemp, Warning, TEXT("Could not uncompress image data."));
        }
    }
    else {
        //UE_LOG(LogTemp, Warning, TEXT("There is a problem with the image data."));
    }
}

void WebSocketActor::ApplyTextureToMaterial(UTexture2D* Texture)
{
    //GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("ApplyTextureToMaterial called."));
    if (DynamicMaterialInstance)
    {
        DynamicMaterialInstance->SetTextureParameterValue(FName("DynamicTextureParam"), Texture);
        //GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Texture applied."));
    }
}
void WebSocketActor::ConvertStringToJson(const FString& JsonString)
{
    UE_LOG(LogTemp, Log, TEXT("Message: %s"), *JsonString);
    // Create a JSON Reader to process the JSON string
    TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);

    // Declare a shared pointer to hold the JSON object
    TSharedPtr<FJsonObject> JsonObject;

    // Deserialize the JSON string into the JSON object
    if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
    {
        // Successfully converted the string to JSON, now you can access the JSON object
        UE_LOG(LogTemp, Log, TEXT("Successfully converted the string to JSON"));
        /*FString SomeValue;
        if (JsonObject->HasField("someKey"))
        {
            SomeValue = JsonObject->GetStringField("someKey");
            UE_LOG(LogTemp, Log, TEXT("Value of someKey: %s"), *SomeValue);
        }*/
    }
    else
    {
        // Handle failure
        UE_LOG(LogTemp, Warning, TEXT("Failed to deserialize JSON string"));
    }
}