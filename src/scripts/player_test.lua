-- Script pour player : Réduit dégâts de tous projectiles créés par le player à 1 (anti one-shot), plus effets précédents

function onInit(self, entity)
    print("[Lua] Init pour player entity " .. entity .. " - Script actif ! Dégâts projectiles réduits à 1.")
    self.boostFactor = 1.5  -- Multiplicateur vitesse low HP
    self.specialShootChance = 0.1  -- 10% chance tir spécial
    self.boostedCooldown = 0.25  -- Cooldown réduit (optionnel, garde pour test)
end

function onUpdate(self, dt, entity)
    local pos = getPosition(entity)
    local vel = getVelocity(entity)
    local health = getHealth(entity)
    local controller = getPlayerController(entity)
    local drawable = getDrawable(entity)

    if pos and vel and health and controller then
        -- Boost vitesse si low HP
        if health.current < health.max / 3 then
            vel.x = vel.x * self.boostFactor
            vel.y = vel.y * self.boostFactor
        end

        -- Tir accéléré : Override cooldown sur shoot
        if controller.isShooting and controller.shootCooldown <= 0 then
            print("[Lua] Tir accéléré ! Set cooldown à " .. self.boostedCooldown)
            controller.shootCooldown = self.boostedCooldown  -- Force à 0.25s

            -- Crée projectile normal, mais réduit dégâts à 1 (anti one-shot)
            local proj = createProjectile(ECS, pos.x + 64, pos.y + 20, 800, 0, 1, 1, "bullet", -1)  -- Damage forcé à 1
            local projComp = getProjectile(proj)
            if projComp then
                projComp.damage = 1  -- Double-check set à 1
                print("[Lua] Projectile créé avec dégâts réduits à 1 !")
            end

            if drawable then
                drawable.scale = 1.2  -- Grossit pour confirmation visuelle
            end
        end

        -- Tir spécial aléatoire (aussi avec dégâts 1)
        if controller.isShooting and math.random() < self.specialShootChance then
            local specProj = createProjectile(ECS, pos.x + 64, pos.y + 20, 800, 0, 1, 1, "bullet_special", -1)  -- Damage 1
            local specComp = getProjectile(specProj)
            if specComp then
                specComp.damage = 1
            end
            controller.shootCooldown = 1.0  -- Long cooldown pour spécial
        end
    end

    -- Flag pour réseau
    local update = getSendUpdate(entity)
    if update then
        update.needsUpdate = true
    end
end

function onCollision(self, otherEntity)
    print("[Lua] Player " .. entity .. " collision avec " .. otherEntity)
    -- Heal sur power-up (team=0)
    local otherCollider = getCollider(otherEntity)
    if otherCollider and otherCollider.team == 0 then
        local health = getHealth(entity)
        if health then
            health.current = math.min(health.current + 20, health.max)
        end
        ECS:killEntity(otherEntity)
    end
end