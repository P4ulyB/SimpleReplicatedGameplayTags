// Copyright SuperSupremeGames

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "TP_Components_GameplayTags.generated.h"

/** Delegate to notify when gameplay tags have changed */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameplayTagsChanged);

/**
 * Actor Component for managing and replicating gameplay tags.
 * This component can be attached to NPCs, player characters, or player controllers.
 * It provides Blueprint-callable functions so that clients can request to add or remove tags;
 * server RPCs then update the replicated tag container.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TEST_API UGameplayTagComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UGameplayTagComponent();

    /** 
     * Client-side: Request to add a gameplay tag.
     * If called on a client, this sends a request to the server.
     */
    UFUNCTION(BlueprintCallable, Category = "Custom Gameplay Tags")
    void RequestAddGameplayTag(const FGameplayTag& NewTag);

    /**
     * Client-side: Request to remove a gameplay tag.
     */
    UFUNCTION(BlueprintCallable, Category = "Custom Gameplay Tags")
    void RequestRemoveGameplayTag(const FGameplayTag& TagToRemove);

    /**
     * Client-side: Request to add multiple gameplay tags.
     */
    UFUNCTION(BlueprintCallable, Category = "Custom Gameplay Tags")
    void RequestAddGameplayTags(const FGameplayTagContainer& NewTags);

    /**
     * Client-side: Request to remove multiple gameplay tags.
     */
    UFUNCTION(BlueprintCallable, Category = "Custom Gameplay Tags")
    void RequestRemoveGameplayTags(const FGameplayTagContainer& TagsToRemove);

    /** Check if this component currently has the specified gameplay tag. */
    UFUNCTION(BlueprintCallable, Category = "Custom Gameplay Tags")
    bool HasGameplayTag(const FGameplayTag& Tag) const;

    /** Check if this component has all of the tags in the given container. */
    UFUNCTION(BlueprintCallable, Category = "Custom Gameplay Tags")
    bool HasAllGameplayTags(const FGameplayTagContainer& TagsToCheck) const;

    /** Check if this component has any of the tags in the given container. */
    UFUNCTION(BlueprintCallable, Category = "Custom Gameplay Tags")
    bool HasAnyGameplayTags(const FGameplayTagContainer& TagsToCheck) const;

    /** Returns the full gameplay tag container. */
    UFUNCTION(BlueprintCallable, Category = "Custom Gameplay Tags")
    FGameplayTagContainer GetAllGameplayTags() const;

    /** Delegate called when the gameplay tags are updated via replication. */
    UPROPERTY(BlueprintAssignable, Category = "Custom Gameplay Tags")
    FOnGameplayTagsChanged OnGameplayTagsChanged;

protected:
    /** Replicated container of gameplay tags. */
    UPROPERTY(ReplicatedUsing = OnRep_GameplayTags, VisibleAnywhere, BlueprintReadOnly, Category = "Custom Gameplay Tags")
    FGameplayTagContainer GameplayTagContainer;

    /** Called on clients when GameplayTags is updated via replication. */
    UFUNCTION()
    void OnRep_GameplayTags();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    // --- Server RPCs ---

    /** Server RPC: Request to add a gameplay tag. */
    UFUNCTION(Server, Reliable, WithValidation)
    void ServerAddGameplayTag(const FGameplayTag& NewTag);

    /** Server RPC: Request to remove a gameplay tag. */
    UFUNCTION(Server, Reliable, WithValidation)
    void ServerRemoveGameplayTag(const FGameplayTag& TagToRemove);

    /** Server RPC: Request to add multiple gameplay tags. */
    UFUNCTION(Server, Reliable, WithValidation)
    void ServerAddGameplayTags(const FGameplayTagContainer& NewTags);

    /** Server RPC: Request to remove multiple gameplay tags. */
    UFUNCTION(Server, Reliable, WithValidation)
    void ServerRemoveGameplayTags(const FGameplayTagContainer& TagsToRemove);

    // --- Internal functions to update tags on the server ---

    /** Adds a gameplay tag on the server; duplicate tags are ignored. Returns true if added. */
    bool AddGameplayTag_Internal(const FGameplayTag& NewTag);

    /** Removes a gameplay tag on the server. Returns true if removed. */
    bool RemoveGameplayTag_Internal(const FGameplayTag& TagToRemove);

    /** Adds multiple gameplay tags on the server; returns true if any tag was added. */
    bool AddGameplayTags_Internal(const FGameplayTagContainer& NewTags);

    /** Removes multiple gameplay tags on the server; returns true if any tag was removed. */
    bool RemoveGameplayTags_Internal(const FGameplayTagContainer& TagsToRemove);

    /** Helper to force a network update on the owning actor if a change occurred. */
    void MarkDirtyForReplication();
    
    /** Enhanced validation: Checks if a tag is allowed. */
    bool IsTagAllowed(const FGameplayTag& Tag) const;
};