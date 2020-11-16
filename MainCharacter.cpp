// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "WeaponKatana.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Enemy.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"
#include "MainPlayerController.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// �޼�
	LeftHandCombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftHandCombatCollision"));
	LeftHandCombatCollision->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("LeftHandSocket"));

	// ������
	RightHandCombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightHandCombatCollision"));
	RightHandCombatCollision->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("RightHandSocket"));

	//�޹�
	LeftTobaseCombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftTobaseCombatCollision"));
	LeftTobaseCombatCollision->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("LeftToeBaseSocket"));

	//������
	RightTobaseCombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightTobaseCombatCollision"));
	RightTobaseCombatCollision->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("RightToeBaseSocket"));

	// ȸ���ӵ� 
	BaseTurnRate = 65.f;
	BaseLookupRate = 65.f;

	// ȸ���ÿ� ȸ���� �������ϵ��ϸ��� (ī�޶� ���� �ش�) 
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// ĳ���� ������ �Է� 
	GetCharacterMovement()->bOrientRotationToMovement = true; // ĳ���Ͱ� �Է¹޴� �������� ������
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.f, 0.0f);
	
	// ���� 
	GetCharacterMovement()->JumpZVelocity = 150.f;
	GetCharacterMovement()->AirControl = 0.2;

	bLMBDown = false;

	InterpSpeed = 15.f;
	bInterpToEnemy = false;	
	bHasCombatTarget = false;

	bShiftKeyDown = false;

	MaxHealth = 300.f;
	Health = 65.f;

	MaxStamina = 300.f;
	Stamina = 100.f;

	PlayerExp = 0.f;
	PlayerMaxExp = 60.f;

	MovementStatus = EMovementStatus::EMS_Idle;

	StaminaStatus = EStaminaStatus::ESS_Normal;

	Damage = 20.f;

	PlayerLevel = 1;

	ComboAttack_num = 0;

	StaminaDrainRate = 25.f;
	MinSprintStamina = 50.f;
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	LeftHandCombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AMainCharacter::PlayerCombatOnOverlapBegin);
	LeftHandCombatCollision->OnComponentEndOverlap.AddDynamic(this, &AMainCharacter::PlayerCombatOnOverlapEnd);

	RightHandCombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AMainCharacter::PlayerCombatOnOverlapBegin);
	RightHandCombatCollision->OnComponentEndOverlap.AddDynamic(this, &AMainCharacter::PlayerCombatOnOverlapEnd);

	LeftTobaseCombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AMainCharacter::PlayerCombatOnOverlapBegin);
	LeftTobaseCombatCollision->OnComponentEndOverlap.AddDynamic(this, &AMainCharacter::PlayerCombatOnOverlapEnd);

	RightTobaseCombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AMainCharacter::PlayerCombatOnOverlapBegin);
	RightTobaseCombatCollision->OnComponentEndOverlap.AddDynamic(this, &AMainCharacter::PlayerCombatOnOverlapEnd);

	LeftHandCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftHandCombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	LeftHandCombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftHandCombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	RightHandCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightHandCombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightHandCombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightHandCombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	LeftTobaseCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftTobaseCombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	LeftTobaseCombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftTobaseCombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	RightTobaseCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightTobaseCombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightTobaseCombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightTobaseCombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	MainPlayerController = Cast<AMainPlayerController>(GetController());
	
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float DeltaStamina = StaminaDrainRate * DeltaTime;

	switch (StaminaStatus)
	{
	case EStaminaStatus::ESS_Normal:
		if (bShiftKeyDown)
		{
			if (Stamina - DeltaStamina <= MinSprintStamina)
			{
				SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
				Stamina -= DeltaStamina;
			}
			else
			{
				Stamina -= DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Sprinting);
		}
		else
		{
			if (Stamina + DeltaStamina >= MaxStamina)
			{
				Stamina = MaxStamina;
			}
			else
			{
				Stamina += DeltaStamina;
			}
			SetStaminaStatus(EStaminaStatus::ESS_Normal);
		}
		break;

	case EStaminaStatus::ESS_BelowMinimum:
		if (bShiftKeyDown)
		{
			if (Stamina - DeltaStamina <= 0.f)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
				Stamina = 0;
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			else
			{
				Stamina -= DeltaStamina;
				SetMovementStatus(EMovementStatus::EMS_Sprinting);
			}
		}
		else
		{
			if (Stamina + DeltaStamina >= MinSprintStamina)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Normal);
				Stamina += DeltaStamina;
			}
			else
			{
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;

	case EStaminaStatus ::ESS_Exhausted:
		if (bShiftKeyDown)
		{
			Stamina = 0.f;
		}
		else
		{
			SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
			Stamina += DeltaStamina;
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;

	case EStaminaStatus::ESS_ExhaustedRecovering:
		if (Stamina + DeltaStamina >= MinSprintStamina)
		{
			SetStaminaStatus(EStaminaStatus::ESS_Normal);
			Stamina += DeltaStamina;
		}
		else
		{
			Stamina += DeltaStamina;
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;

	default:
		;

	}


	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	if (bInterpToEnemy && CombatTarget)
	{
		FRotator LookAtYaw = GetLookAtRotaionYaw(CombatTarget->GetActorLocation());
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);
		SetActorRotation(InterpRotation);
	}

	if (CombatTarget)
	{
		CombatTargetLocation = CombatTarget->GetActorLocation();
		if (MainPlayerController)
		{
			MainPlayerController->EnemyLocation = CombatTargetLocation;
		}
	}
}

FRotator AMainCharacter::GetLookAtRotaionYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);

	return LookAtRotationYaw;
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("JUMP",IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("JUMP", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Attack", IE_Pressed,this, &AMainCharacter::AttackMelee);
	PlayerInputComponent->BindAction("Attack", IE_Released, this, &AMainCharacter::AttackMeleeEnd);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAxis("TurnRate", this, &AMainCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMainCharacter::LookUpAtRate);

	// ���� ��ü ��ư 
	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMainCharacter::LMBDown);
	PlayerInputComponent->BindAction("LMB", IE_Released, this, &AMainCharacter::LMBup);

}

void AMainCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f) && (isDuringAttack==false) && (MovementStatus != EMovementStatus::EMS_Dead))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
		ComboAttack_num = 0;
	}
}

void AMainCharacter::MoveRight(float Value)
{

	if ((Controller != nullptr) && (Value != 0.0f) && (isDuringAttack==false) && (MovementStatus != EMovementStatus::EMS_Dead))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
		ComboAttack_num = 0;
	}
}

void AMainCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookupRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacter::LMBDown()
{
	bLMBDown = true;

	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	// ���� ��ü 
	if (ActiveOverlappingItem)
	{
		AWeaponKatana* katana = Cast<AWeaponKatana>(ActiveOverlappingItem);
		if (katana)
		{
			katana->Equip(this);
			SetActiveOverlapping(nullptr);
		}
	}
	else if (Equippedweapon)
	{
		AttackMelee();
	}
}
// ���ο� ���� ������ �ı� 
void AMainCharacter::SetEquippedWeapon(AWeaponKatana* WeaponToSet)
{
	if (Equippedweapon)
	{
		Equippedweapon->Destroy();
	}
	Equippedweapon = WeaponToSet;
}

void AMainCharacter::AttackMelee()
{
	if (!isDuringAttack && MovementStatus != EMovementStatus::EMS_Dead)
	{
		isDuringAttack = true;
		SetInterpToEnemy(true);
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (ComboAttack_num < 3)
		{
			int tmp_num = rand() % 3 + 1;
			FString PlayerSection = "Attack_" + FString::FromInt(tmp_num);
			PlayAnimMontage(AttackCombo, 1.f, FName(*PlayerSection));
			ComboAttack_num++;
			
		}
		else
		{
			PlayAnimMontage(LastAttackCombo, 1.f);
			ComboAttack_num = 0;
		}

	}
	FTimerHandle TH_Attack_End;
	GetWorldTimerManager().SetTimer(TH_Attack_End, this, &AMainCharacter::AttackMeleeEnd, 1.7f, false);


	/*
	switch (ComboAttack_num)
	{
		case 0:
		PlayAnimMontage(AttackMeleeAnim, 1.0f);
		isDuringAttack = true;
		ComboAttack_num++;
		break;

	case 1 :
		PlayAnimMontage(AttackMeleeAnimCombo, 1.2f);
		isDuringAttack = true;
		ComboAttack_num++;
		break;
	case 2:
		PlayAnimMontage(AttackMeleeAnimCombo2, 1.3f);
		isDuringAttack = true;
		ComboAttack_num++;
		break;

	default:
		ComboAttack_num = 0;
	}
	*/
	
}

void AMainCharacter::AttackMeleeEnd()
{
	isDuringAttack = false;
	SetInterpToEnemy(false);
	if (bLMBDown)
	{
		AttackMelee();
	}

}
bool AMainCharacter::CanSetWeapon()
{
	return (nullptr == Equippedweapon);
}


void AMainCharacter::PlayerCombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor)
	{
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy)
		{
			if (Enemy->EnemyHitParticle)
			{
				const USkeletalMeshSocket* LeftHandParticleSocket = GetMesh()->GetSocketByName("LeftHandParticleSocket");
				const USkeletalMeshSocket* RightHandParticleSocket = GetMesh()->GetSocketByName("RightHandParticleSocket");
				const USkeletalMeshSocket* RightToeBaseParticleSocket = GetMesh()->GetSocketByName("RightToeBaseParticleSocket");
				
				if (LeftHandParticleSocket)
				{
					FVector SocketLocation = LeftHandParticleSocket->GetSocketLocation(GetMesh());
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Enemy->EnemyHitParticle, SocketLocation, FRotator(0.f), false);
				}
				else if (RightHandParticleSocket)
				{
					FVector SocketLocations = LeftHandParticleSocket->GetSocketLocation(GetMesh());
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Enemy->EnemyHitParticle, SocketLocations, FRotator(0.f), false);
				}
				else if (RightToeBaseParticleSocket)
				{
					FVector SocketLocationd = LeftHandParticleSocket->GetSocketLocation(GetMesh());
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Enemy->EnemyHitParticle, SocketLocationd, FRotator(0.f), false);
				}
			}
			if (Enemy->EnemyLastHitParticle)
			{
				const USkeletalMeshSocket* LeftToeBaseParticleSocket = GetMesh()->GetSocketByName("LeftToeBaseParticleSocket");
				if (LeftToeBaseParticleSocket)
				{
					FVector LastSocketLocation = LeftToeBaseParticleSocket->GetSocketLocation(GetMesh());
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Enemy->EnemyLastHitParticle, LastSocketLocation, FRotator(0.f), false);
				}
			}
			if (Enemy->PlayerHitSound)
			{
				UGameplayStatics::PlaySound2D(this, Enemy->PlayerHitSound);
			}
			if (DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(Enemy, Damage, PauchInstigator,this, DamageTypeClass);
			}
			
		}
	}
}

void AMainCharacter::PlayerCombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
	if (OtherActor)
	{
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);

		if (Enemy)
		{
			// �÷��̾� ����ġ ȹ��
			if (Enemy->Health == 0)
			{
				PlayerExp += Enemy->Exp;
			}
		}
		PlayerExpBar();
	}
	//PlayerExpBar();
	
}

void AMainCharacter::Die()
{
	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackCombo)
	{
		AnimInstance->Montage_Play(AttackCombo, 1.0f);
		AnimInstance->Montage_JumpToSection(FName("Death"));
	}
	SetMovementStatus(EMovementStatus::EMS_Dead);
}

/*
void AMainCharacter::Jump()
{
	if (MovementStatus != EMovementStatus::EMS_Dead)
	{
		Super::Jump();

	}
}
*/
void AMainCharacter::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
}

void AMainCharacter::DecrementHealth(float Amount)
{
	if (Health - Amount <= 0.f)
	{
		Health -= Amount;
		Die();
	}
	else
	{
		Health -= Amount;
	}
}


void AMainCharacter::ActivateCollision()
{
	LeftHandCombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AMainCharacter::DeactivateCollision()
{
	LeftHandCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMainCharacter::ActivateCollisiontwo()
{
	RightHandCombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AMainCharacter::DeactivateCollisiontwo()
{
	RightHandCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMainCharacter::ActivateCollisionthree()
{
	RightTobaseCombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AMainCharacter::DeactivateCollisionthree()
{
	RightTobaseCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMainCharacter::LastActivateCollision()
{
	LeftTobaseCombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}


void AMainCharacter::LastDeactivateCollision()
{
	LeftTobaseCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


void AMainCharacter::SetInterpToEnemy(bool Interp)
{
	bInterpToEnemy = Interp;
}



float AMainCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser)
{
	if (Health - DamageAmount <= 0.f)
	{
		Health -= DamageAmount;
		Die();
		if (DamageCauser)
		{
			AEnemy* Enemy = Cast<AEnemy>(DamageCauser);
			if (Enemy)
			{
				Enemy->bHasValidTarget = false;
			}
		}
	}
	else
	{
		Health -= DamageAmount;
	}

	return DamageAmount;
}

void AMainCharacter::PlayerExpBar()
{
	if (PlayerExp >= PlayerMaxExp)
	{
		PlayerMaxExp -= PlayerExp;
	}
}
void AMainCharacter::PlayerLevelText(int32 Level)
{
	if (PlayerExp >= PlayerMaxExp)
	{
		PlayerLevel += Level;
	}
}
	

	
	



/*
// ���ڿ��� ���¹��� ���� 
void AMainCharacter::SetWeapon(class AWeaponKatana* NewWeapon)
{
	FName WeaponSocket(TEXT("RightHandSocket"));
	if (nullptr != NewWeapon)
	{
		NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
		NewWeapon->SetOwner(this);
		Equippedweapon = NewWeapon;
	}
}
*/