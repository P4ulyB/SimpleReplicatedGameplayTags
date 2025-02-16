UGameplayTagComponent System Documentation
1. Introduction: 
Lets be clear before we proceed, this code isn't groundbreaking by any means, it's created as a means for convenience so you dont have to do this yourself. The UGameplayTagComponent system is a modular Unreal Engine component designed for managing and replicating gameplay tags in a multiplayer environment. It allows clients to query, add, and remove gameplay tags from actors with this component, while ensuring server authority over modifications.
2. Features
- **Replication:** Maintains a replicated gameplay tag container across server and clients.
- **Batch Operations:** Supports adding or removing multiple tags at once.
- **Server Authority:** Only the server can modify gameplay tags.
- **Client Requests:** Clients can request tag modifications via RPCs.
- **Delegates:** Notifies listeners when gameplay tags change.
- **Blueprint Integration:** Exposes query and request functions to Blueprints.
3. Component Structure
3.1 Header File (TP_Components_GameplayTags.h)
**Properties:**
- `FGameplayTagContainer GameplayTagContainer`: Holds replicated gameplay tags.
- `FOnGameplayTagsChanged OnGameplayTagsChanged`: Delegate for tag change events.

**Blueprint Functions:**
- `RequestAddGameplayTag`: Requests to add a tag.
- `RequestRemoveGameplayTag`: Requests to remove a tag.
- `RequestAddGameplayTags`: Requests to add multiple tags.
- `RequestRemoveGameplayTags`: Requests to remove multiple tags.
- `HasGameplayTag`, `HasAllGameplayTags`, `HasAnyGameplayTags`: Queries current tags.

**Server RPCs:**
- `ServerAddGameplayTag`, `ServerRemoveGameplayTag`: Single-tag operations.
- `ServerAddGameplayTags`, `ServerRemoveGameplayTags`: Batch operations.

**Internal Functions:**
- `AddGameplayTag_Internal`, `RemoveGameplayTag_Internal`: Handles server-side logic.
- `MarkDirtyForReplication`: Forces network updates.
- `OnRep_GameplayTags`: Reacts to replication updates.
3.2 Source File (TP_Components_GameplayTags.cpp)
**Client Requests:**
- Client calls `RequestAddGameplayTag` or `RequestAddGameplayTags`.
- Non-authoritative clients send `ServerAddGameplayTag` or `ServerAddGameplayTags` RPCs.

**Server RPC Handling:**
- Validates tags using `ServerAddGameplayTag_Validate`.
- Adds tags via `AddGameplayTag_Internal`.
- Calls `MarkDirtyForReplication` if tags changed.

**Replication:**
- `GameplayTagContainer` is replicated using `DOREPLIFETIME`.
- `OnRep_GameplayTags` broadcasts the `OnGameplayTagsChanged` delegate.

4. Adding to Player Controller
To add `UGameplayTagComponent` to `PA_PlayerController`:
**In `PA_PlayerController.h`**:
```cpp
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (DisplayName = "GameplayTagComponent"))
TObjectPtr<UGameplayTagComponent> GameplayTagComponent;
```

**In `PA_PlayerController.cpp`**:
```cpp
GameplayTagComponent = CreateDefaultSubobject<UGameplayTagComponent>(TEXT("GameplayTagComponent"));
GameplayTagComponent->SetupAttachment(RootComponent);
```

5. Best Practices
- Use `RequestAddGameplayTag` and `RequestRemoveGameplayTag` for client modifications.
- Subscribe to `OnGameplayTagsChanged` for gameplay reactions.
- Add custom validations in `ServerAddGameplayTag_Validate`.
- Use `GetAllGameplayTags()` to query tags efficiently.

6. Example Blueprint Usage
In Blueprints:
- Call `RequestAddGameplayTag` to add tags from abilities or triggers.
- Bind `OnGameplayTagsChanged` to update the HUD.
- Use `HasGameplayTag` to enable or disable abilities.

7. Conclusion
The `UGameplayTagComponent` system is designed to provide a robust solution for managing and querying gameplay tags in a multiplayer environment. It ensures server authority, efficient network replication, and easy integration with Blueprints for gameplay logic.
