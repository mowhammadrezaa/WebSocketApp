// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WebSocketsModule.h"
#include "WebSocketActor.generated.h"
//#include "Json.h"
//#include "Dom/JsonObject.h"
//#include "Serialization/JsonReader.h"
//#include "Serialization/JsonSerializer.h"


UCLASS()
class TESTMMPOSEAPI_API WebSocketActor : public AActor
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Sets default values for this actor's properties
	WebSocketActor();

	int32 MessageCount;
	FTimerHandle MessageCountTimerHandle;

private:
	TSharedPtr<IWebSocket> WebSocket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WebSocket", meta = (AllowPrivateAccess = "true"))
	FString WebSocketURL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WebSocket", meta = (AllowPrivateAccess = "true"))
	FString WebSocketGroupID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WebSocket", meta = (AllowPrivateAccess = "true"))
	UMaterialInstanceDynamic* DynamicMaterialInstance;

	void OnConnected();
	void OnConnectionError(const FString& Error);
	void OnMessage(const FString& Message);
	void OnBinaryMessage(const void *Data, SIZE_T Size, bool bIsLastFragment);
	void OnRawMessage(const void *Data, SIZE_T Size, SIZE_T BytesRemaining);
	void OnClosed(int32 StatusCode, const FString& Reason, bool bWasClean);
	void ResetAndLogMessageCount();
	void ConvertStringToJson(const FString& JsonString);

	void ConvertBinaryToTexture(const TArray<uint8>& BinaryData);
	void ApplyTextureToMaterial(UTexture2D* Texture);
	//UFUNCTION(BlueprintCallable, Category = "WebSocket")

	// Static variable to accumulate binary data across multiple calls
	static TArray<uint8> AccumulatedData;

	//// Mutex for thread safety
	//static FCriticalSection Mutex;
};