deployment
apiVersion: apps/v1
kind: Deployment
metadata:
  name: second-app-deployment
spec:
  replicas: 3
  selector:
    matchLabels:
      app: second-app
      tier: backend
  template:
    metadata:
      labels:
        app: second-app
        tier: backend
    spec:
      containers:
      - name: second-node
        image: jbite9057/kub-first-app
      # - name: ...
      #   image: ...
        # resources:
        #   limits:
        #     memory: "128Mi"
        #     cpu: "500m"
        # ports:
        # - containerPort: 8080


service
apiVersion: v1
kind: Service
metadata:
    name: backend
spec:
    selector:
        app: second-app
    ports:
        - protocol: 'TCP'
          port: 80
          targetPort: 8080
        # - protocol: 'TCP'
        #   port: 443
        #   targetPort: 443
    type: LoadBalancer



應用yaml檔案
kubectl apply -f=deployment

刪除
kubectl delete -f=deployment -f=service

使用label刪除
kubectl delete deployments,services -l group=example

啟用kube容器服務
minikube service [deployment.metadata.name]
