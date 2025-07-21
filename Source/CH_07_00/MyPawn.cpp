// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPawn.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

// Sets default values
AMyPawn::AMyPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 캡슐 컴포넌트를 만들고 -> RootComponent에 붙인다.
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleCompnent"));
	RootComponent = CapsuleComponent;

	// 과제 요구사항
	CapsuleComponent->SetSimulatePhysics(false);

	// 스켈레탈 메시 컴포넌트를 생성하고 캡슐에 부착
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	// 요구사항 -> 물리 시뮬레이션 OFF
	Mesh->SetSimulatePhysics(false);

	// 스프링 암을 생성하고 캡슐에 부착 (셀카봉)
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.0f; // 카메라와의 거리

	// 카메라를 생성하고 스프링 암 끝에 부착
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}

// Called when the game starts or when spawned
void AMyPawn::BeginPlay()
{
	Super::BeginPlay();

	// 게임 시작 시 Enhanced Input 시스템을 활성화합니다.
	// 우리가 만든 IMC를 엔진에 등록한다
	// Pawn에 있는 Controller를 -> 플레이어용 PlayerController로 변환! AI라면 AI용으로 변환!
	// 언리얼에서 입력은 Subsystem에서 관리
	// 현재 로컬 플레이어의 서브시스템을 가져와서 에디터에서 만든 IMC와 연결합니다.
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AMyPawn::Move(const FInputActionValue& Value)
{
	// WASD 입력값을 변수에 저장
	CurrentMoveInput = Value.Get<FVector2D>(); // (1.0, 0)
}

void AMyPawn::StopMove(const FInputActionValue& Value)
{
	CurrentMoveInput = FVector2D::ZeroVector; // (0.0, 0.0)
}

void AMyPawn::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	// 좌우  (1.0, 0.0)

	// AddControllerInput 대신 직접 Actor 회전
	// 좌/우 회전(Yaw)은 Pawn 전체를 직접 회전
	// GetWorld()->GetDeltaSeconds() -> tick의 DeltaTime과 같은 값
	AddActorLocalRotation(FRotator(0.0f, LookAxisVector.X * LookSpeed * GetWorld()->GetDeltaSeconds(), 0.0f));

	// 위/아래 회전(pitch)은 카메라가 달려있는 스프링 암을 회 -> 캐릭터가 앞/뒤로 움직이면 안됩니다.	
	FRotator CurrentSpringArmRotation = SpringArm->GetRelativeRotation(); // 현재 SpringArm의 rotation 가져오기
	// 현재 Pitch에서 -  마우스의 위 아래 * 회전 속도 * 프레임 간격
	float NewPitch = CurrentSpringArmRotation.Pitch - LookAxisVector.Y * LookSpeed * GetWorld()->GetDeltaSeconds();

	// 카메라가 땅을 파고들거나 하늘만 보지 않도록 각도를 제한
	NewPitch = FMath::Clamp(NewPitch, -75.f, 0.f);

	SpringArm->SetRelativeRotation(FRotator(NewPitch, 0.f, 0.f));
}

void AMyPawn::Jump(const FInputActionValue& Value)
{
	// 지상에 체크 후 점프 가능 하도록 설정
	if (bIsOnGround)
	{
		Velocity.Z = JumpForce; // Z에 force 대입
		bIsOnGround = false; // 공중 상태로 표시
	}
}
// Called every frame
void AMyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 1. 지면 감지 (LineTrace)
	FHitResult HitResult;
	FVector StartTrace = GetActorLocation();
	// 캡슐 높이 + 여유
	FVector EndTrace = StartTrace + FVector(0.f, 0.f, -(CapsuleComponent->GetScaledCapsuleHalfHeight() + 10.0f));
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	bIsOnGround = GetWorld()->
		LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECC_Visibility, CollisionParams);

	// 디버그 라인 (테스트용, 녹색: 착지, 빨강: 공중)
	DrawDebugLine(
		GetWorld(),
		StartTrace,
		EndTrace,
		bIsOnGround ? FColor::Green : FColor::Red,
		false,
		0.0f,
		0,
		1.0f
	);

	// 2. 중력 및 속도 계산
	if (!bIsOnGround)
	{
		Velocity.Z += Gravity * DeltaTime; // 중력 적용 (낙하)
	}

	// 3. 이동 입력 계산 (공중/지상 구분)
	float CurrentMoveSpeed = bIsOnGround ? MoveSpeed : MoveSpeed * AirControl;
	FVector DesiredMovement = FVector(CurrentMoveInput.X, CurrentMoveInput.Y, 0.0f).GetSafeNormal() * CurrentMoveSpeed;

	// 지상: 즉시 속도 적용 / 공중: 부드럽게 추가 (에어컨트롤)
	if (bIsOnGround)
	{
		Velocity.X = DesiredMovement.X;
		Velocity.Y = DesiredMovement.Y;
		if (Velocity.Z < 0.f) Velocity.Z = 0.f; // 지상에서 아래로 가지 않음
	}
	else
	{
		Velocity.X = FMath::Lerp(Velocity.X, DesiredMovement.X, AirControl); // 부드럽게
		Velocity.Y = FMath::Lerp(Velocity.Y, DesiredMovement.Y, AirControl);
	}

	// 4. 착지 처리
	if (bIsOnGround && Velocity.Z < 0.0f)
	{
		Velocity.Z = 0.0f; // 속도 초기화
		// 위치 조정 (뚫림 방지)
		SetActorLocation(HitResult.ImpactPoint + FVector(0.0f, 0.0f, CapsuleComponent->GetScaledCapsuleHalfHeight()));
	}

	// 5. 최종 이동 적용 (로컬 -> 월드 변환)
	FVector WorldVelocity = GetActorRotation().RotateVector(Velocity);
	AddActorWorldOffset(WorldVelocity * DeltaTime, true); // Sweep으로 충돌 체크
}


// 입력 -> Enhanced Input Subsystem -> PlayerInput -> PlayerController(SetupPlayerInputComponent) -> 바인딩된 함수 -> 이동
void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// IA_Move 액션이 발동하면 이 클래스의 Move 함수를 호출하도록 바인딩!
		EnhancedInputComponent->BindAction(
			MoveAction, // MoveAction이 
			ETriggerEvent::Triggered, // 발동되면
			this, // 현재 객체
			&AMyPawn::Move // Move함수를 실행
		);
		EnhancedInputComponent->BindAction(
			MoveAction,
			ETriggerEvent::Completed, // Completed -> StopMove실행
			this,
			&AMyPawn::StopMove
		);
		EnhancedInputComponent->BindAction(
			LookAction,
			ETriggerEvent::Triggered,
			this,
			&AMyPawn::Look
		);
		EnhancedInputComponent->BindAction(
			JumpAction,
			ETriggerEvent::Triggered,
			this,
			&AMyPawn::Jump
		);
	}
}