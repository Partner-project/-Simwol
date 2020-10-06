// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"

UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
	EMS_Idle UMETA(DisplayName = "Idle"),
	EMS_Dead UMETA(DisplayName ="Dead")
};

UCLASS()
class OPENRPG_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Controller")
	class AMainPlayerController* MainPlayerController;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadwrite, Category = "Combat")
	class UBoxComponent* LeftHandCombatCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadwrite, Category = "Combat")
	class UBoxComponent* RightHandCombatCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadwrite, Category = "Combat")
	class UBoxComponent* LeftTobaseCombatCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadwrite, Category = "Combat")
	class UBoxComponent* RightTobaseCombatCollision;

	// �÷��̾ �ǰ��� �������� ���´� ����Ʈ 
	UPROPERTY(EditAnywhere,BlueprintReadwrite,Category = "Combat")
	class UParticleSystem* PlayerHitParticles;

	// ���Ͱ� �÷��̾� ������ ���� 
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Combat")
	class USoundCue* EnemyHitSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category ="Combat")
	class AEnemy* CombatTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookupRate;

	// PlayerStat
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Player Stat")
	float MaxHealth;
	
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Player Stat")
	float Health;

	//UPROPERTY(EditAnywhere,BlueprintReadwrite,Category = "Player Stat")
	//float Exp;
	
	//UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Player Stat")
	//float MaxExp;

	int ComboAttack_num;

	UPROPERTY(EditDefaultsOnly,Category = Attack)
	UAnimMontage* AttackMeleeAnim;

	UPROPERTY(EditDefaultsOnly, Category = Attack)
	UAnimMontage* AttackMeleeAnimCombo;

	UPROPERTY(EditDefaultsOnly, Category = Attack)
	UAnimMontage* AttackMeleeAnimCombo2;

	UPROPERTY(EditDefaultsOnly, Category = Attack)
	UAnimMontage* AttackCombo;
	
	UPROPERTY(EditDefaultsOnly, Category = Attack)
	UAnimMontage* LastAttackCombo;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "AI")
	float Damage;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void TurnAtRate(float Rate);
	
	void LookUpAtRate(float Rate);

	void AttackMelee();

	void DecrementHealth(float Amount);

	void Die();

	FORCEINLINE void SetMovementStatus(EMovementStatus Status) { MovementStatus = Status; }

	virtual float TakeDamage(float DamageAmount,struct FDamageEvent const & DamageEvent,class AController * EventInstigator,AActor * DamageCauser) override;

	//virtual void Jump() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadwrite, Category = "Enums")
	EMovementStatus MovementStatus;

	UPROPERTY(VisibleAnywhere,BlueprintReadwrite,Category = "Combat")
	FVector CombatTargetLocation;

	UFUNCTION(BlueprintCallable)
	void ActivateCollision();

	UFUNCTION(BlueprintCallable)
	void DeactivateCollision();

	UFUNCTION(BlueprintCallable)
	void ActivateCollisiontwo();

	UFUNCTION(BlueprintCallable)
	void DeactivateCollisiontwo();

	UFUNCTION(BlueprintCallable)
	void ActivateCollisionthree();

	UFUNCTION(BlueprintCallable)
	void DeactivateCollisionthree();

	UFUNCTION(BlueprintCallable)
	void LastActivateCollision();

	UFUNCTION(BlueprintCallable)
	void LastDeactivateCollision();

	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	UFUNCTION(BlueprintCallable)
	void AttackMeleeEnd();

	void LMBDown();
	void LMBup() { bLMBDown = false; }
	bool bLMBDown;

	// ���� ���� ����ġ 
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Anims")
	bool isDuringAttack;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category ="Combat")
	bool bHasCombatTarget;

	// ���� ���� true/false
	bool CanSetWeapon();

	// ������ ���� (�����Ҷ� ���� ������ ��) 
	float InterpSpeed;

	bool bInterpToEnemy;

	void SetInterpToEnemy(bool Interp);

	FRotator GetLookAtRotaionYaw(FVector Target);

	//void SetWeapon(class AWeaponKatana* NewWeapon);

	//���� ����(?)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = items)
	class AWeaponKatana* Equippedweapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = items)
	class Aitem* ActiveOverlappingItem;

	//������ ���� ����
	void SetEquippedWeapon(AWeaponKatana* WeaponToSet);
	FORCEINLINE AWeaponKatana* GetEquippedWeapon() { return Equippedweapon; }

	//���� ��ü ����
	FORCEINLINE void SetActiveOverlapping(Aitem* item) { ActiveOverlappingItem = item; }

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera ; }

	// �� ���� ���� 
	FORCEINLINE void SetCombatTarget(AEnemy* Target) { CombatTarget = Target; }

	UFUNCTION()
	virtual void PlayerCombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	virtual void PlayerCombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UPROPERTY(EditAnywhere,BlueprintReadwrite,Category = "Combat")
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category ="Combat")
	AController* PauchInstigator;

	FORCEINLINE void SetInstigator(AController* Inst) { PauchInstigator = Inst; }

	FORCEINLINE void SetHasCombatTarget(bool HasTarget) { bHasCombatTarget = HasTarget; }
	
	


};
