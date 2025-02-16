// Copyright


#include "Components/TP_Components_GameplayTags.h"
#include "Net/UnrealNetwork.h"

UGameplayTagComponent::UGameplayTagComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UGameplayTagComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UGameplayTagComponent, GameplayTagContainer);
}

//////////////////////////////
// Client-side Request Functions
//////////////////////////////

void UGameplayTagComponent::RequestAddGameplayTag(const FGameplayTag& NewTag)
{
    if (GetOwnerRole() < ROLE_Authority)
    {
        ServerAddGameplayTag(NewTag);
    }
    else
    {
        if (AddGameplayTag_Internal(NewTag))
        {
            MarkDirtyForReplication();
        }
    }
}

void UGameplayTagComponent::RequestRemoveGameplayTag(const FGameplayTag& TagToRemove)
{
    if (GetOwnerRole() < ROLE_Authority)
    {
        ServerRemoveGameplayTag(TagToRemove);
    }
    else
    {
        if (RemoveGameplayTag_Internal(TagToRemove))
        {
            MarkDirtyForReplication();
        }
    }
}

void UGameplayTagComponent::RequestAddGameplayTags(const FGameplayTagContainer& NewTags)
{
    if (GetOwnerRole() < ROLE_Authority)
    {
        ServerAddGameplayTags(NewTags);
    }
    else
    {
        if (AddGameplayTags_Internal(NewTags))
        {
            MarkDirtyForReplication();
        }
    }
}

void UGameplayTagComponent::RequestRemoveGameplayTags(const FGameplayTagContainer& TagsToRemove)
{
    if (GetOwnerRole() < ROLE_Authority)
    {
        ServerRemoveGameplayTags(TagsToRemove);
    }
    else
    {
        if (RemoveGameplayTags_Internal(TagsToRemove))
        {
            MarkDirtyForReplication();
        }
    }
}

//////////////////////////////
// Server RPCs - Single Tag
//////////////////////////////

bool UGameplayTagComponent::ServerAddGameplayTag_Validate(const FGameplayTag& NewTag)
{
    // Enhanced validation: Ensure the tag is valid and allowed.
    return NewTag.IsValid() && IsTagAllowed(NewTag);
}

void UGameplayTagComponent::ServerAddGameplayTag_Implementation(const FGameplayTag& NewTag)
{
    if (AddGameplayTag_Internal(NewTag))
    {
        MarkDirtyForReplication();
    }
}

bool UGameplayTagComponent::ServerRemoveGameplayTag_Validate(const FGameplayTag& TagToRemove)
{
    return TagToRemove.IsValid() && IsTagAllowed(TagToRemove);
}

void UGameplayTagComponent::ServerRemoveGameplayTag_Implementation(const FGameplayTag& TagToRemove)
{
    if (RemoveGameplayTag_Internal(TagToRemove))
    {
        MarkDirtyForReplication();
    }
}

//////////////////////////////
// Server RPCs - Batch Operations
//////////////////////////////

bool UGameplayTagComponent::ServerAddGameplayTags_Validate(const FGameplayTagContainer& NewTags)
{
    // Validate each tag in the container.
    for (const FGameplayTag& Tag : NewTags)
    {
        if (!Tag.IsValid() || !IsTagAllowed(Tag))
        {
            return false;
        }
    }
    return true;
}

void UGameplayTagComponent::ServerAddGameplayTags_Implementation(const FGameplayTagContainer& NewTags)
{
    if (AddGameplayTags_Internal(NewTags))
    {
        MarkDirtyForReplication();
    }
}

bool UGameplayTagComponent::ServerRemoveGameplayTags_Validate(const FGameplayTagContainer& TagsToRemove)
{
    for (const FGameplayTag& Tag : TagsToRemove)
    {
        if (!Tag.IsValid() || !IsTagAllowed(Tag))
        {
            return false;
        }
    }
    return true;
}

void UGameplayTagComponent::ServerRemoveGameplayTags_Implementation(const FGameplayTagContainer& TagsToRemove)
{
    if (RemoveGameplayTags_Internal(TagsToRemove))
    {
        MarkDirtyForReplication();
    }
}

//////////////////////////////
// Internal Tag Modification (Server Only)
//////////////////////////////

bool UGameplayTagComponent::AddGameplayTag_Internal(const FGameplayTag& NewTag)
{
    if (!GetOwner() || !GetOwner()->HasAuthority())
    {
        return false;
    }

    if (GameplayTagContainer.HasTag(NewTag))
    {
        return false;
    }

    GameplayTagContainer.AddTag(NewTag);
    return true;
}

bool UGameplayTagComponent::RemoveGameplayTag_Internal(const FGameplayTag& TagToRemove)
{
    if (!GetOwner() || !GetOwner()->HasAuthority())
    {
        return false;
    }

    if (!GameplayTagContainer.HasTag(TagToRemove))
    {
        return false;
    }

    GameplayTagContainer.RemoveTag(TagToRemove);
    return true;
}

bool UGameplayTagComponent::AddGameplayTags_Internal(const FGameplayTagContainer& NewTags)
{
    if (!GetOwner() || !GetOwner()->HasAuthority())
    {
        return false;
    }

    bool bChanged = false;
    for (const FGameplayTag& Tag : NewTags)
    {
        if (!GameplayTagContainer.HasTag(Tag))
        {
            GameplayTagContainer.AddTag(Tag);
            bChanged = true;
        }
    }
    return bChanged;
}

bool UGameplayTagComponent::RemoveGameplayTags_Internal(const FGameplayTagContainer& TagsToRemove)
{
    if (!GetOwner() || !GetOwner()->HasAuthority())
    {
        return false;
    }

    bool bChanged = false;
    for (const FGameplayTag& Tag : TagsToRemove)
    {
        if (GameplayTagContainer.HasTag(Tag))
        {
            GameplayTagContainer.RemoveTag(Tag);
            bChanged = true;
        }
    }
    return bChanged;
}

//////////////////////////////
// Tag Query Functions
//////////////////////////////

bool UGameplayTagComponent::HasGameplayTag(const FGameplayTag& Tag) const
{
    return GameplayTagContainer.HasTag(Tag);
}

bool UGameplayTagComponent::HasAllGameplayTags(const FGameplayTagContainer& TagsToCheck) const
{
    return GameplayTagContainer.HasAll(TagsToCheck);
}

bool UGameplayTagComponent::HasAnyGameplayTags(const FGameplayTagContainer& TagsToCheck) const
{
    return GameplayTagContainer.HasAny(TagsToCheck);
}

FGameplayTagContainer UGameplayTagComponent::GetAllGameplayTags() const
{
    return GameplayTagContainer;
}

//////////////////////////////
// Replication Notification
//////////////////////////////

void UGameplayTagComponent::OnRep_GameplayTags()
{
    // Broadcast delegate to notify listeners of tag changes.
    OnGameplayTagsChanged.Broadcast();
}

//////////////////////////////
// Helper for Replication Updates
//////////////////////////////

void UGameplayTagComponent::MarkDirtyForReplication()
{
    if (AActor* Owner = GetOwner())
    {
        // Only force update if the owner exists.
        Owner->ForceNetUpdate();
    }
}

//////////////////////////////
// Enhanced Validation: Allowed Tag Check
//////////////////////////////

bool UGameplayTagComponent::IsTagAllowed(const FGameplayTag& Tag) const
{
    // Example validation: Only allow tags that meet a specific condition.
    // For demonstration purposes, we allow all valid tags.
    return Tag.IsValid();
}
